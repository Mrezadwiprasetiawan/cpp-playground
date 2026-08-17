/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <array>
#include <cmath>
#include <iostream>
#include <matrix.hxx>
#include <nn_objects.hxx>
#include <random>
#include <vec.hxx>

using FP                 = double;
constexpr size_t D_MODEL = 8;
constexpr size_t SEQ_LEN = 5;
constexpr size_t D_FF    = 16;
constexpr size_t VOCAB   = 12;

using Linear::Mat;
using Linear::Vec;

template <int Rows, int Cols>
static void randomize(Mat<FP, Rows, Cols> &m, std::mt19937 &gen, FP scale) {
  std::normal_distribution<FP> dist(0.0, scale);
  FP                          *d = m.data();
  for (size_t i = 0; i < Rows * Cols; ++i) d[i] = dist(gen);
}

template <int N>
static void randomize(Vec<FP, N> &v, std::mt19937 &gen, FP scale) {
  std::normal_distribution<FP> dist(0.0, scale);
  for (size_t i = 0; i < N; ++i) v[i] = dist(gen);
}

template <int Rows, int Cols>
static void add_bias_rows(Mat<FP, Rows, Cols> &m, const Vec<FP, Cols> &b) {
  for (size_t i = 0; i < Rows; ++i)
    for (size_t j = 0; j < Cols; ++j) m.set_element(i * Cols + j, m[i][j] + b[j]);
}

template <int Rows, int Cols>
static void relu_inplace(Mat<FP, Rows, Cols> &m) {
  FP *d = m.data();
  for (size_t i = 0; i < static_cast<size_t>(Rows * Cols); ++i) d[i] = d[i] > 0 ? d[i] : static_cast<FP>(1e-6);
}

template <int Rows, int Cols>
static Mat<FP, Rows, Cols> softmax_rows(const Mat<FP, Rows, Cols> &m) {
  Mat<FP, Rows, Cols> res;
  for (size_t i = 0; i < Rows; ++i) {
    Vec<FP, Cols> row  = m[i];
    Vec<FP, Cols> soft = NN::softmax(row);
    for (size_t j = 0; j < Cols; ++j) res.set_element(i * Cols + j, soft[j]);
  }
  return res;
}

template <int Rows, int Cols>
static Mat<FP, Rows, Cols> layer_norm(const Mat<FP, Rows, Cols> &m, FP epsilon = 1e-6) {
  Mat<FP, Rows, Cols> res;
  for (size_t i = 0; i < Rows; ++i) {
    Vec<FP, Cols> row  = m[i];
    FP            mean = 0;
    for (size_t j = 0; j < Cols; ++j) mean += row[j];
    mean /= static_cast<FP>(Cols);

    FP var = 0;
    for (size_t j = 0; j < Cols; ++j) var += (row[j] - mean) * (row[j] - mean);
    var /= static_cast<FP>(Cols);

    FP invStd = 1.0 / std::sqrt(var + epsilon);
    for (size_t j = 0; j < Cols; ++j) res.set_element(i * Cols + j, (row[j] - mean) * invStd);
  }
  return res;
}

struct EncoderBlock {
  Mat<FP, D_MODEL, D_MODEL> Wq, Wk, Wv, Wo;
  Mat<FP, D_FF, D_MODEL>    W1;
  Vec<FP, D_FF>             b1;
  Mat<FP, D_MODEL, D_FF>    W2;
  Vec<FP, D_MODEL>          b2;

  void init(std::mt19937 &gen) {
    FP scale = static_cast<FP>(1.0 / std::sqrt(static_cast<FP>(D_MODEL)));
    randomize(Wq, gen, scale);
    randomize(Wk, gen, scale);
    randomize(Wv, gen, scale);
    randomize(Wo, gen, scale);
    randomize(W1, gen, scale);
    randomize(W2, gen, scale);
    randomize(b1, gen, scale);
    randomize(b2, gen, scale);
  }

  Mat<FP, SEQ_LEN, D_MODEL> forward(const Mat<FP, SEQ_LEN, D_MODEL> &x) const {
    Mat<FP, SEQ_LEN, D_MODEL> q = x * Wq;
    Mat<FP, SEQ_LEN, D_MODEL> k = x * Wk;
    Mat<FP, SEQ_LEN, D_MODEL> v = x * Wv;

    Mat<FP, D_MODEL, SEQ_LEN> kT          = k.transposed();
    Mat<FP, SEQ_LEN, SEQ_LEN> scores      = q * kT;
    FP                        scaleFactor = static_cast<FP>(1.0 / std::sqrt(static_cast<FP>(D_MODEL)));
    scores                                = scores * scaleFactor;

    Mat<FP, SEQ_LEN, SEQ_LEN> attn    = softmax_rows(scores);
    Mat<FP, SEQ_LEN, D_MODEL> attnOut = attn * v;
    Mat<FP, SEQ_LEN, D_MODEL> selfOut = attnOut * Wo;

    Mat<FP, SEQ_LEN, D_MODEL> x2 = layer_norm(x + selfOut);

    Mat<FP, D_MODEL, D_FF> W1t    = W1.transposed();
    Mat<FP, SEQ_LEN, D_FF> hidden = x2 * W1t;
    add_bias_rows(hidden, b1);
    relu_inplace(hidden);

    Mat<FP, D_FF, D_MODEL>    W2t   = W2.transposed();
    Mat<FP, SEQ_LEN, D_MODEL> ffOut = hidden * W2t;
    add_bias_rows(ffOut, b2);

    return layer_norm(x2 + ffOut);
  }
};

static Mat<FP, SEQ_LEN, D_MODEL> positional_encoding() {
  Mat<FP, SEQ_LEN, D_MODEL> pe;
  for (size_t pos = 0; pos < SEQ_LEN; ++pos) {
    for (size_t i = 0; i < D_MODEL; i += 2) {
      FP freq  = std::pow(10000.0, -static_cast<FP>(i) / static_cast<FP>(D_MODEL));
      FP angle = static_cast<FP>(pos) * freq;
      pe.set_element(pos * D_MODEL + i, std::sin(angle));
      if (i + 1 < D_MODEL) pe.set_element(pos * D_MODEL + i + 1, std::cos(angle));
    }
  }
  return pe;
}

int main() {
  std::mt19937 gen(42);

  Mat<FP, VOCAB, D_MODEL> embedding;
  randomize(embedding, gen, static_cast<FP>(1.0 / std::sqrt(static_cast<FP>(D_MODEL))));

  Mat<FP, D_MODEL, VOCAB> outputProj;
  randomize(outputProj, gen, static_cast<FP>(1.0 / std::sqrt(static_cast<FP>(D_MODEL))));

  EncoderBlock block;
  block.init(gen);

  std::array<size_t, SEQ_LEN> tokens{1, 4, 7, 2, 9};

  Mat<FP, SEQ_LEN, D_MODEL> x;
  for (size_t pos = 0; pos < SEQ_LEN; ++pos)
    for (size_t j = 0; j < D_MODEL; ++j) x.set_element(pos * D_MODEL + j, embedding[tokens[pos]][j]);

  x = x + positional_encoding();

  Mat<FP, SEQ_LEN, D_MODEL> encoded = block.forward(x);
  Mat<FP, SEQ_LEN, VOCAB>   logits  = encoded * outputProj;
  Mat<FP, SEQ_LEN, VOCAB>   probs   = softmax_rows(logits);

  for (size_t pos = 0; pos < SEQ_LEN; ++pos) {
    Vec<FP, VOCAB> row       = probs[pos];
    size_t         bestToken = 0;
    FP             bestProb  = row[0];
    for (size_t t = 1; t < VOCAB; ++t) {
      if (row[t] > bestProb) {
        bestProb  = row[t];
        bestToken = t;
      }
    }
    std::cout << "pos " << pos << " input_token=" << tokens[pos] << " predicted_token=" << bestToken << " prob=" << bestProb << std::endl;
  }

  return 0;
}
