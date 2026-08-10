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

#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
// #include <thread>

namespace NN {

/* MAE = Mean Absolute Error
 * MSE = Mean Squared Error
 */
enum LOSS_TYPE { MAE, MSE, CROSS_ENTROPY };
enum ACTIVATION_TYPE { RELU, SIGMOID, TANH };

// perlu dimasukan ke parameter template karena semua array di dalamnya statis
template <std::floating_point FP, size_t inputSize, size_t hidden1Size, size_t hidden2Size, size_t outputSize>
class BasicFFN {
  ACTIVATION_TYPE    act_t;
  LOSS_TYPE          loss_t;
  FP               **wIn, **wHid1, **wHid2;
  FP               **gwIn, **gwHid1, **gwHid2;  // akumulator gradient bobot, dipakai buat training batch
  FP                *bIn, *bHid1, *bHid2;
  FP                *gbIn, *gbHid1, *gbHid2;  // akumulator gradient bias, dipakai buat training batch
  FP                *toHid1, *toHid2, *out;
  FP                *dOut, *dHid2, *dHid1;
  FP                 lastLoss         = -1;
  std::string        weights_filename = "";
  inline static FP   epsilon          = 1e-6;  // untuk mencegah dead neuron ketika menggunakan ReLU
  bool               xavier           = false;
  FP                 eta              = 1e-2;
  std::random_device rd;
  std::mt19937       gen;
  // FP = Floating Point @param FP1 current eta/learning rate @param FP2 grad
  FP (*adaptive_eta_func)(FP, FP);

  using ActFn  = FP (*)(FP);
  using LossFn = FP (*)(FP, FP);

  // setiap layer punya distribusi yang berbeda
  template <size_t inSize, size_t outSize>
  void init_layer(FP k, FP **w, FP *b) {
    // setup normal distribution
    std::normal_distribution<FP> dis(0, std::sqrt(2 / k));

    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) w[j][i] = dis(gen);
      b[i] = 0;
    }
  }

  /* He Initialization cuma make k = input
   * sedangkan xavier init make k = input + output;
   */
  void init_wb() {
    // setup random number generator
    gen.seed(rd());
    FP k0 = inputSize, k1 = hidden1Size, k2 = hidden2Size;
    if (xavier) {
      k0 += hidden1Size;
      k1 += hidden2Size;
      k2 += outputSize;
    }
    init_layer<inputSize, hidden1Size>(k0, wIn, bIn);
    init_layer<hidden1Size, hidden2Size>(k1, wHid1, bHid1);
    init_layer<hidden2Size, outputSize>(k2, wHid2, bHid2);
  }

  FP **allocate_2d(size_t rows, size_t cols) {
    FP **mat = new FP *[rows];
    for (size_t i = 0; i < rows; ++i) mat[i] = new FP[cols]{};
    return mat;
  }

  void deallocate_2d(FP **mat, size_t rows) {
    for (size_t i = 0; i < rows; ++i) delete[] mat[i];
    delete[] mat;
  }

  // Activation func
  static FP ReLU(FP x) { return x > 0 ? x : epsilon; }
  static FP ReLU_deriv(FP y) { return y > 0 ? 1 : epsilon; }
  static FP sigmoid(FP x) { return 1 / (1 + std::exp(-x)); }
  static FP sigmoid_deriv(FP y) { return y * (1 - y); }
  static FP tanh(FP x) { return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x)); }
  static FP tanh_deriv(FP y) { return 1 - y * y; }

  // pemetaan enum ke pointer fungsi aktivasi/loss, dipakai bareng oleh forward maupun backward (single & batch)
  ActFn act_func_of(ACTIVATION_TYPE t) {
    switch (t) {
      case ACTIVATION_TYPE::RELU: return ReLU;
      case ACTIVATION_TYPE::SIGMOID: return sigmoid;
      case ACTIVATION_TYPE::TANH: return tanh;
    }
    return ReLU;
  }

  ActFn act_deriv_of(ACTIVATION_TYPE t) {
    switch (t) {
      case ACTIVATION_TYPE::RELU: return ReLU_deriv;
      case ACTIVATION_TYPE::SIGMOID: return sigmoid_deriv;
      case ACTIVATION_TYPE::TANH: return tanh_deriv;
    }
    return ReLU_deriv;
  }

  LossFn loss_func_of(LOSS_TYPE t) {
    switch (t) {
      case LOSS_TYPE::MAE: return MAE;
      case LOSS_TYPE::MSE: return MSE;
      case LOSS_TYPE::CROSS_ENTROPY: return cross_entropy_loss;
    }
    return MSE;
  }

  LossFn loss_deriv_of(LOSS_TYPE t) {
    switch (t) {
      case LOSS_TYPE::MAE: return MAE_deriv;
      case LOSS_TYPE::MSE: return MSE_deriv;
      case LOSS_TYPE::CROSS_ENTROPY: return cross_entropy_loss_deriv;
    }
    return MSE_deriv;
  }

  /* Agar lebih mudah dibaca, layer layernya diabstraksi jadi fungsi independen.
  Untuk performa sih lebih baik di unwrap karena nambah overhead untuk
  pemanggilan fungsi kalau diabstraksi semua. Lebih bagus lagi kalau di taruh di
  shader compute GPU, atau pake Tensorflownya CUDA sekalian. Next time bakal
  diunwrap
  */
  // fungsi untuk forward per layer, @actFunc pointer fungsi aktivasi atau 0
  // alias nullptr jika tidak
  template <size_t inSize, size_t outSize>
  void forward_layer(FP **w, FP *b, FP *dataIn, FP *dataOut, FP (*actFunc)(FP)) {
    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) dataOut[i] += w[j][i] * dataIn[j];
      dataOut[i] += b[i];
      if (actFunc) dataOut[i] = actFunc(dataOut[i]);
    }
  }

  /* fungsi untuk backward per layer, langsung update bobot & bias (dipakai versi single-sample)
  @param inSize parameter template untuk ukuran layer masuk (dari belakang)
  @param outSize parameter template untuk ukuran layer keluar (dari belakang)
  @param actFuncDeriv pointer ke turunan fungsi aktivasi
   */
  template <size_t inSize, size_t outSize>
  void backward_layer(FP **w, FP *b, FP *dataIn, FP *deltaIn, FP *(*deltaOut), FP (*actFuncDeriv)(FP), FP eta) {
    // update bobot dan bias berdasarkan delta in
    for (size_t i = 0; i < inSize; ++i) {
      for (size_t j = 0; j < outSize; ++j) w[j][i] -= eta * deltaIn[i] * dataIn[j];
      b[i] -= eta * deltaIn[i];
    }

    if (!deltaOut) return;
    // update delta Out jika disediakan
    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) (*deltaOut)[i] += w[i][j] * deltaIn[j];
      // aturan rantai
      if (actFuncDeriv) (*deltaOut)[i] *= actFuncDeriv(dataIn[i]);
    }
  }

  /* fungsi untuk backward per layer versi batch: TIDAK langsung update bobot & bias,
  cuma akumulasi gradiennya ke gw/gb. Bobot yang dipakai buat propagasi delta ke layer
  sebelumnya juga masih bobot lama (belum diupdate), karena update baru dilakukan sekali
  di akhir batch lewat apply_gradients().
  @param inSize parameter template untuk ukuran layer masuk (dari belakang)
  @param outSize parameter template untuk ukuran layer keluar (dari belakang)
  @param actFuncDeriv pointer ke turunan fungsi aktivasi
   */
  template <size_t inSize, size_t outSize>
  void backward_layer_accum(FP **w, FP **gw, FP *gb, FP *dataIn, FP *deltaIn, FP *(*deltaOut), FP (*actFuncDeriv)(FP)) {
    for (size_t i = 0; i < inSize; ++i) {
      for (size_t j = 0; j < outSize; ++j) gw[j][i] += deltaIn[i] * dataIn[j];
      gb[i] += deltaIn[i];
    }

    if (!deltaOut) return;
    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) (*deltaOut)[i] += w[i][j] * deltaIn[j];
      if (actFuncDeriv) (*deltaOut)[i] *= actFuncDeriv(dataIn[i]);
    }
  }

  // apply rata-rata gradient hasil akumulasi satu batch ke bobot & bias, lalu reset akumulatornya ke 0
  void apply_gradients(size_t batchSize) {
    auto apply_w = [&](FP **w, FP **gw, size_t rows, size_t cols) {
      for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j) {
          w[i][j]  -= eta * gw[i][j] / batchSize;
          gw[i][j]  = 0;
        }
    };
    auto apply_b = [&](FP *b, FP *gb, size_t size) {
      for (size_t i = 0; i < size; ++i) {
        b[i]  -= eta * gb[i] / batchSize;
        gb[i]  = 0;
      }
    };

    apply_w(wIn, gwIn, inputSize, hidden1Size);
    apply_w(wHid1, gwHid1, hidden1Size, hidden2Size);
    apply_w(wHid2, gwHid2, hidden2Size, outputSize);
    apply_b(bIn, gbIn, hidden1Size);
    apply_b(bHid1, gbHid1, hidden2Size);
    apply_b(bHid2, gbHid2, outputSize);
  }

  void write_matrix(std::ofstream &ofs, FP **mat, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; ++i) ofs.write(reinterpret_cast<char *>(mat[i]), sizeof(FP) * cols);
  }

  void read_matrix(std::ifstream &ifs, FP **mat, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; ++i) ifs.read(reinterpret_cast<char *>(mat[i]), sizeof(FP) * cols);
  }

 public:
  // default activation using ReLU
  explicit BasicFFN(ACTIVATION_TYPE act_t = ACTIVATION_TYPE::RELU, LOSS_TYPE loss_t = LOSS_TYPE::MSE) : act_t(act_t), loss_t(loss_t) {
    wIn    = allocate_2d(inputSize, hidden1Size);
    wHid1  = allocate_2d(hidden1Size, hidden2Size);
    wHid2  = allocate_2d(hidden2Size, outputSize);
    gwIn   = allocate_2d(inputSize, hidden1Size);
    gwHid1 = allocate_2d(hidden1Size, hidden2Size);
    gwHid2 = allocate_2d(hidden2Size, outputSize);
    bIn    = new FP[hidden1Size];
    bHid1  = new FP[hidden2Size];
    bHid2  = new FP[outputSize];
    gbIn   = new FP[hidden1Size]{};
    gbHid1 = new FP[hidden2Size]{};
    gbHid2 = new FP[outputSize]{};
    toHid1 = new FP[hidden1Size];
    toHid2 = new FP[hidden2Size];
    out    = new FP[outputSize]{};
    dHid1  = new FP[hidden1Size];
    dHid2  = new FP[hidden2Size];
    dOut   = new FP[outputSize];
    init_wb();
  }
  explicit BasicFFN(ACTIVATION_TYPE act_t, LOSS_TYPE loss_t, std::string weightfilename) : act_t(act_t), loss_t(loss_t), weights_filename(weightfilename) {
    wIn    = allocate_2d(inputSize, hidden1Size);
    wHid1  = allocate_2d(hidden1Size, hidden2Size);
    wHid2  = allocate_2d(hidden2Size, outputSize);
    gwIn   = allocate_2d(inputSize, hidden1Size);
    gwHid1 = allocate_2d(hidden1Size, hidden2Size);
    gwHid2 = allocate_2d(hidden2Size, outputSize);
    bIn    = new FP[hidden1Size];
    bHid1  = new FP[hidden2Size];
    bHid2  = new FP[outputSize];
    gbIn   = new FP[hidden1Size]{};
    gbHid1 = new FP[hidden2Size]{};
    gbHid2 = new FP[outputSize]{};
    toHid1 = new FP[hidden1Size];
    toHid2 = new FP[hidden2Size];
    out    = new FP[outputSize]{};
    dHid1  = new FP[hidden1Size];
    dHid2  = new FP[hidden2Size];
    dOut   = new FP[outputSize];
    load_weights();
  }

  // set epsilon if using ReLU to avoid dead neuron, @param epsilon default 1e-6
  void set_epsilon(FP epsilon) { BasicFFN::epsilon = epsilon; }
  void set_learning_rate(FP eta) { this->eta = eta; }
  void set_adaptive_learning_rate_func(FP (*adaptive_eta_func)(FP, FP)) { this->adaptive_eta_func = adaptive_eta_func; }

  /* return output in array, versi data tunggal
  @note array will be lost after class is destroyed
  */
  FP *forward(FP *data) {
    // zeroing data layer agar tidak menumpuk
    std::fill(toHid1, toHid1 + hidden1Size, 0);
    std::fill(toHid2, toHid2 + hidden2Size, 0);
    std::fill(out, out + outputSize, 0);

    auto actFunc = act_func_of(act_t);

    // input data to the input layer
    forward_layer<inputSize, hidden1Size>(wIn, bIn, data, toHid1, 0);
    // hidden layer
    forward_layer<hidden1Size, hidden2Size>(wHid1, bHid1, toHid1, toHid2, actFunc);
    forward_layer<hidden2Size, outputSize>(wHid2, bHid2, toHid2, out, actFunc);

    return out;
  }

  /* versi batch, forward tiap baris di dataBatch lalu hasilnya disalin ke outBatch
  @param dataBatch array of pointer, ukuran [batchSize][inputSize]
  @param outBatch array of pointer yang sudah dialokasikan pemanggil, ukuran [batchSize][outputSize]
  @note dipakai kalau outputnya perlu disimpan semua sekaligus (forward tunggal reuse buffer internal
  yang sama, jadi hasilnya ketimpa tiap panggilan)
  */
  void forward(FP **dataBatch, FP **outBatch, size_t batchSize) {
    for (size_t s = 0; s < batchSize; ++s) {
      FP *res = forward(dataBatch[s]);
      std::copy(res, res + outputSize, outBatch[s]);
    }
  }

  // Loss function section
  // Mean Absolute Error
  static FP MAE(FP ypred, FP y) { return std::abs(ypred - y); }
  // Derivative of Mean Absolute Error
  static FP MAE_deriv(FP ypred, FP y) { return (ypred > y) ? 1 : (ypred < y) ? -1 : 0; }
  // Mean Squared Error
  static FP MSE(FP ypred, FP y) {
    FP semi_loss = ypred - y;
    return 0.5 * semi_loss * semi_loss;
  }
  // Derivative of Mean Squared Error
  static FP MSE_deriv(FP ypred, FP y) { return ypred - y; }

  static FP cross_entropy_loss(FP ypred, FP y) { return -y * std::log(ypred + epsilon) - (1 - y) * std::log(1 - ypred + epsilon); }
  static FP cross_entropy_loss_deriv(FP ypred, FP y) { return (ypred - y) / ((ypred + epsilon) * (1 - ypred + epsilon)); }

  /* using w = wcurr - eta * dL/dw
   using b = bcurr - eta * dL/db;
   versi data tunggal, langsung update bobot & bias tiap dipanggil
   */
  void backward(FP *inputData, FP *targetData) {
    forward(inputData);
    // zeroing delta layer agar tidak menumpuk
    std::fill(dOut, dOut + outputSize, 0);
    std::fill(dHid2, dHid2 + hidden2Size, 0);
    std::fill(dHid1, dHid1 + hidden1Size, 0);

    auto actFuncDeriv  = act_deriv_of(act_t);
    auto lossDerivFunc = loss_deriv_of(loss_t);
    auto lossFunc      = loss_func_of(loss_t);

    // calculate dOut first for update through backward_layer template function
    lastLoss = 0;
    for (size_t i = 0; i < outputSize; ++i) {
      lastLoss += lossFunc(out[i], targetData[i]) / outputSize;
      dOut[i]   = lossDerivFunc(out[i], targetData[i]) * actFuncDeriv(out[i]);
    }

    backward_layer<outputSize, hidden2Size>(wHid2, bHid2, toHid2, dOut, &dHid2, actFuncDeriv, eta);
    backward_layer<hidden2Size, hidden1Size>(wHid1, bHid1, toHid1, dHid2, &dHid1, actFuncDeriv, eta);
    backward_layer<hidden1Size, inputSize>(wIn, bIn, inputData, dHid1, 0, 0, eta);
  }

  /* versi batch, gradient dari semua sample diakumulasi dulu (pakai bobot yang sama, belum diupdate),
  baru di-apply sekali di akhir sebagai rata-rata. Ini beda dari backward tunggal yang update bobot
  langsung tiap sample.
  @param inputBatch array of pointer, ukuran [batchSize][inputSize]
  @param targetBatch array of pointer, ukuran [batchSize][outputSize]
  */
  void backward(FP **inputBatch, FP **targetBatch, size_t batchSize) {
    auto actFuncDeriv  = act_deriv_of(act_t);
    auto lossDerivFunc = loss_deriv_of(loss_t);
    auto lossFunc      = loss_func_of(loss_t);

    lastLoss = 0;
    for (size_t s = 0; s < batchSize; ++s) {
      forward(inputBatch[s]);

      std::fill(dOut, dOut + outputSize, 0);
      std::fill(dHid2, dHid2 + hidden2Size, 0);
      std::fill(dHid1, dHid1 + hidden1Size, 0);

      for (size_t i = 0; i < outputSize; ++i) {
        lastLoss += lossFunc(out[i], targetBatch[s][i]) / (outputSize * batchSize);
        dOut[i]   = lossDerivFunc(out[i], targetBatch[s][i]) * actFuncDeriv(out[i]);
      }

      backward_layer_accum<outputSize, hidden2Size>(wHid2, gwHid2, gbHid2, toHid2, dOut, &dHid2, actFuncDeriv);
      backward_layer_accum<hidden2Size, hidden1Size>(wHid1, gwHid1, gbHid1, toHid1, dHid2, &dHid1, actFuncDeriv);
      backward_layer_accum<hidden1Size, inputSize>(wIn, gwIn, gbIn, inputBatch[s], dHid1, 0, 0);
    }

    apply_gradients(batchSize);
  }

  FP get_loss() { return lastLoss; }

  void set_weights_filename(const std::string &filename) {
    weights_filename = filename;
    if (std::filesystem::exists(weights_filename)) load_weights();
  }

  void save_weights() {
    std::ofstream ofs(weights_filename, std::ios::binary);
    if (!ofs) {
      std::cerr << "Failed to open file for saving: " << weights_filename << "\n";
      return;
    }
    write_matrix(ofs, wIn, inputSize, hidden1Size);
    write_matrix(ofs, wHid1, hidden1Size, hidden2Size);
    write_matrix(ofs, wHid2, hidden2Size, outputSize);

    ofs.write(reinterpret_cast<char *>(bIn), sizeof(FP) * hidden1Size);
    ofs.write(reinterpret_cast<char *>(bHid1), sizeof(FP) * hidden2Size);
    ofs.write(reinterpret_cast<char *>(bHid2), sizeof(FP) * outputSize);
  }

  void load_weights() {
    std::ifstream ifs(weights_filename, std::ios::binary);
    if (!ifs) {
      std::cerr << "Failed to open file for loading: " << weights_filename << "\n";
      std::cout << "Fallback to the default init weight" << "\n";
      init_wb();
      return;
    }

    read_matrix(ifs, wIn, inputSize, hidden1Size);
    read_matrix(ifs, wHid1, hidden1Size, hidden2Size);
    read_matrix(ifs, wHid2, hidden2Size, outputSize);

    ifs.read(reinterpret_cast<char *>(bIn), sizeof(FP) * hidden1Size);
    ifs.read(reinterpret_cast<char *>(bHid1), sizeof(FP) * hidden2Size);
    ifs.read(reinterpret_cast<char *>(bHid2), sizeof(FP) * outputSize);
  }

  ~BasicFFN() {
    deallocate_2d(wIn, inputSize);
    deallocate_2d(wHid1, hidden1Size);
    deallocate_2d(wHid2, hidden2Size);
    deallocate_2d(gwIn, inputSize);
    deallocate_2d(gwHid1, hidden1Size);
    deallocate_2d(gwHid2, hidden2Size);
    delete[] bIn;
    delete[] bHid1;
    delete[] bHid2;
    delete[] gbIn;
    delete[] gbHid1;
    delete[] gbHid2;
    delete[] toHid1;
    delete[] toHid2;
    delete[] out;
    delete[] dHid1;
    delete[] dHid2;
    delete[] dOut;
  }
};

}  // namespace NN