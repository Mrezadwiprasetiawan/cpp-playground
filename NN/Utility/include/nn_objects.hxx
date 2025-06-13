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

#include <cassert>
#include <cmath>
#include <type_traits>
#include <vector>

namespace NN {
#define TEMPLATE_FLOAT template <typename FP, typename = std::enable_if_t<std::is_floating_point_v<FP>>>

// Because i just implemented square Matrix but not general Matrix, then i need to implement this general Matrix
TEMPLATE_FLOAT using vec = std::vector<FP>;
TEMPLATE_FLOAT
class Matrix {
  std::vector<std::vector<FP>> data;

 public:
  Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) { data.resize(rows, std::vector<FP>(cols, 0)); }
  Matrix(std::initializer_list<std::initializer_list<FP>> init) {
    for (const auto &row : init) data.push_back(std::vector<FP>(row));
  }
  Matrix(const std::vector<std::vector<FP>> &data) : data(data) {}

  // Accessors
  std::vector<FP> &operator[](size_t row) { return data[row]; }
  FP &operator()(size_t row, size_t col) { return data[row][col]; }
  const FP &operator()(size_t row, size_t col) const { return data[row][col]; }

  // operator overloads
  Matrix operator*(const Matrix &other) const {
    size_t rows = data.size();
    size_t cols = other.data[0].size();
    size_t inner_dim = data[0].size();
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i)
      for (size_t j = 0; j < cols; ++j)
        for (size_t k = 0; k < inner_dim; ++k) result(i, j) += data[i][k] * other(k, j);

    return result;
  }

#define OV_ARITHMETIC_OP(op)                                                     \
  Matrix &operator op(const Matrix & other) {                                    \
    for (size_t i = 0; i < data.size(); ++i)                                     \
      for (size_t j = 0; j < data[i].size(); ++j) data[i][j] op## = other(i, j); \
    \                                                                               
    return *this;                                                                \
  }

  OV_ARITHMETIC_OP(+)
  OV_ARITHMETIC_OP(-)
#undef OV_ARITHMETIC_OP

  vec<FP> operator*(vec<FP> other) {
    assert(data.size() = other size() && "mismatch row size");
    vec<FP> res(data.size(), 0);
    for (size_t row = 0; row < data.size(); ++row)
      for (size_t col = 0; col < data[0].size(); ++col) res[row] += data[row][col] * other[row];

    return res;
  }

  Matrix transpose() const {
    size_t rows = data.size(), cols = data[0].size();
    Matrix result(cols, rows);

    for (size_t i = 0; i < rows; ++i)
      for (size_t j = 0; j < cols; ++j) result(j, i) = data[i][j];

    return result;
  }

#define OV_ASSIGNMENT_OP(op) \
  Matrix &operator op##=(const Matrix & other) { return *this = *this op other; }

  OV_ASSIGNMENT_OP(*)
  OV_ASSIGNMENT_OP(+)
  OV_ASSIGNMENT_OP(-)
#undef OV_ASSIGNMENT_OP

  // Getters
  size_t num_rows() const { return data.size(); }
  size_t num_cols() const { return data[0].size(); }
};

#define OV_OP(op)                                                            \
  TEMPLATE_FLOAT vec<FP> operator op(const vec<FP> lhs, const vec<FP> rhs) { \
    assert(lhs.size() == rhs.size() && "size mismatch");                     \
    vec<FP> res;                                                             \
    for (size_t i = 0; i < lhs.size(); ++i) res.push_back(lhs[i] op rhs[i]); \
    return res;                                                              \
  }
OV_OP(*)
OV_OP(+)
OV_OP(-)
#undef OV_OP

TEMPLATE_FLOAT struct LayerLayout {
  size_t size;
  ACTIVATION_TYPE act_func_t;
};

TEMPLATE_FLOAT struct Layer {
  Matrix<FP> inputs, w, outputs;
  std::vector<FP> b;
};

enum ACTIVATION_TYPE { sigmoid, ReLU, tanh, NONE };
enum LOSS_TYPE { MAE, MSE, cross_entropy };
enum COMPUTE_MODE { CPU, GPU };

#define TEMPLATE_FUNC_FLOAT \
  template <typename FP>    \
  std::enable_if_t<std::is_floating_point_v<FP>>
// Rectified Linear Unit Activation function
TEMPLATE_FUNC_FLOAT ReLU(FP x, FP epsilon) { return x > 0 ? x : epsilon; }
// ReLU derivative function
TEMPLATE_FUNC_FLOAT ReLU_deriv(FP y, FP epsilon) { return y > 0 ? 1 : epsilon; }
// Sigmoid activation function
TEMPLATE_FUNC_FLOAT sigmoid(FP x) { return 1 / (1 + std::exp(-x)); }
// Sigmoid Derivative function
TEMPLATE_FUNC_FLOAT Sigmoid_deriv(FP y) { return y * (1 - y); }
// Tanh activation function
TEMPLATE_FUNC_FLOAT tanh(FP x) { return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x)); }
// Tanh derivative function
TEMPLATE_FUNC_FLOAT tanh_deriv(FP y) { return 1 - y * y; }
// Mean Absolute Error Loss Function
TEMPLATE_FUNC_FLOAT MAE(FP ypred, FP y) { return std::abs(ypred - y); }
// Mean Absolute Error derivative function
TEMPLATE_FUNC_FLOAT MAE_deriv(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return semi_loss > 0 ? 1 : semi_loss == 0 ? 0 : -1;
}
// Mean Squared Error Loss Function
TEMPLATE_FUNC_FLOAT MSE(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return 0.5 * semi_loss * semi_loss;
}
// Mean Squared Error derivative function
TEMPLATE_FUNC_FLOAT MSE_deriv(FP ypred, FP y) { return ypred - y; }
// Cross Entropy Loss Function
TEMPLATE_FUNC_FLOAT cross_entropy(FP ypred, FP y, FP epsilon) { return -y * std::log(ypred + epsilon) - (1 - y) * std::log(1 - ypred + epsilon); }
// Cross Entropy dericative function
TEMPLATE_FUNC_FLOAT cross_entropy_deriv(FP ypred, FP y, FP epsilon) { return (ypred - y) / ((ypred + epsilon) * (1 - ypred + epsilon)); }

}  // namespace NN