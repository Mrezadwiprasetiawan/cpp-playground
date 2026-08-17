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

#include <cmath>
#include <concepts>
#include <cstddef>
#include <vector>

#include "matrix.hxx"
#include "vec.hxx"

namespace NN {

enum class ACTIVATION_TYPE { sigmoid, ReLU, tanh, NONE };
enum class LOSS_TYPE { MAE, MSE, cross_entropy };
enum class COMPUTE_MODE { CPU, GPU };

template <std::floating_point FP, size_t InSize, size_t OutSize>
struct Layer {
  Linear::Mat<FP, OutSize, InSize> w;
  Linear::Vec<FP, OutSize>         b;
  Linear::Vec<FP, InSize>          inputs;
  Linear::Vec<FP, OutSize>         outputs;
  ACTIVATION_TYPE                  act_func_t = ACTIVATION_TYPE::NONE;
};

template <std::floating_point FP>
inline FP ReLU(FP x, FP epsilon) {
  return x > 0 ? x : epsilon;
}

template <std::floating_point FP>
inline FP ReLU_deriv(FP y, FP epsilon) {
  return y > 0 ? 1 : epsilon;
}

template <std::floating_point FP>
inline FP sigmoid(FP x) {
  return 1 / (1 + std::exp(-x));
}

template <std::floating_point FP>
inline FP Sigmoid_deriv(FP y) {
  return y * (1 - y);
}

template <std::floating_point FP>
inline FP tanh(FP x) {
  return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x));
}

template <std::floating_point FP>
inline FP tanh_deriv(FP y) {
  return 1 - y * y;
}

template <std::floating_point FP>
inline FP MAE(FP ypred, FP y) {
  return std::abs(ypred - y);
}

template <std::floating_point FP>
inline FP MAE_deriv(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return semi_loss > 0 ? 1 : semi_loss == 0 ? 0 : -1;
}

template <std::floating_point FP>
inline FP MSE(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return 0.5 * semi_loss * semi_loss;
}

template <std::floating_point FP>
inline FP MSE_deriv(FP ypred, FP y) {
  return ypred - y;
}

template <std::floating_point FP>
inline FP cross_entropy(FP ypred, FP y, FP epsilon) {
  return -y * std::log(ypred + epsilon) - (1 - y) * std::log(1 - ypred + epsilon);
}

template <std::floating_point FP>
inline FP cross_entropy_deriv(FP ypred, FP y, FP epsilon) {
  return (ypred - y) / ((ypred + epsilon) * (1 - ypred + epsilon));
}

template <std::floating_point FP, int N>
inline Linear::Vec<FP, N> apply_activation(const Linear::Vec<FP, N> &v, ACTIVATION_TYPE t, FP epsilon = static_cast<FP>(1e-6)) {
  Linear::Vec<FP, N> res = v;
  for (size_t i = 0; i < N; ++i) {
    switch (t) {
      case ACTIVATION_TYPE::sigmoid: res[i] = sigmoid(v[i]); break;
      case ACTIVATION_TYPE::ReLU: res[i] = ReLU(v[i], epsilon); break;
      case ACTIVATION_TYPE::tanh: res[i] = tanh(v[i]); break;
      case ACTIVATION_TYPE::NONE: break;
    }
  }
  return res;
}

template <std::floating_point FP, int N>
inline Linear::Vec<FP, N> softmax(const Linear::Vec<FP, N> &v) {
  FP maxVal = v[0];
  for (int i = 1; i < N; ++i)
    if (v[i] > maxVal) maxVal = v[i];

  Linear::Vec<FP, N> res;
  FP                 sum = 0;
  for (int i = 0; i < N; ++i) {
    res[i]  = std::exp(v[i] - maxVal);
    sum    += res[i];
  }
  for (int i = 0; i < N; ++i) res[i] /= sum;
  return res;
}

}  // namespace NN
