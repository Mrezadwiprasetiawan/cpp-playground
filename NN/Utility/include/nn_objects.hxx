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
#include <vector>

namespace NN {

template <std::floating_point FP>
struct WeightBias {
  std::vector<FP> w;
  FP              b;
};

template <std::floating_point FP>
struct Layer {
  size_t          size;
  ACTIVATION_TYPE act_func_t;
};

enum ACTIVATION_TYPE { sigmoid, ReLU, tanh, NONE };
enum LOSS_TYPE { MAE, MSE, cross_entropy };
enum COMPUTE_MODE { CPU, GPU };

// Rectified Linear Unit Activation function
template <std::floating_point FP>
FP ReLU(FP x, FP epsilon) {
  return x > 0 ? x : epsilon;
}
// ReLU derivative function
template <std::floating_point FP>
FP ReLU_deriv(FP y, FP epsilon) {
  return y > 0 ? 1 : epsilon;
}
// Sigmoid activation function
template <std::floating_point FP>
FP sigmoid(FP x) {
  return 1 / (1 + std::exp(-x));
}
// Sigmoid Derivative function
template <std::floating_point FP>
FP Sigmoid_deriv(FP y) {
  return y * (1 - y);
}
// Tanh activation function
template <std::floating_point FP>
FP tanh(FP x) {
  return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x));
}
// Tanh derivative function
template <std::floating_point FP>
FP tanh_deriv(FP y) {
  return 1 - y * y;
}
// Mean Absolute Error Loss Function
template <std::floating_point FP>
FP MAE(FP ypred, FP y) {
  return std::abs(ypred - y);
}
// Mean Absolute Error derivative function
template <std::floating_point FP>
FP MAE_deriv(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return semi_loss > 0 ? 1 : semi_loss == 0 ? 0 : -1;
}
// Mean Squared Error Loss Function
template <std::floating_point FP>
FP MSE(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return 0.5 * semi_loss * semi_loss;
}
// Mean Squared Error derivative function
template <std::floating_point FP>
FP MSE_deriv(FP ypred, FP y) {
  return ypred - y;
}
// Cross Entropy Loss Function
template <std::floating_point FP>
FP cross_entropy(FP ypred, FP y, FP epsilon) {
  return -y * std::log(ypred + epsilon) - (1 - y) * std::log(1 - ypred + epsilon);
}
// Cross Entropy dericative function
template <std::floating_point FP>
FP cross_entropy_deriv(FP ypred, FP y, FP epsilon) {
  return (ypred - y) / ((ypred + epsilon) * (1 - ypred + epsilon));
}
}  // namespace NN