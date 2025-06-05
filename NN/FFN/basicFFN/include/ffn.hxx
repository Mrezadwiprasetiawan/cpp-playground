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

#include <cstddef>
#include <random>
#include <thread>
#include <type_traits>

namespace NN {

/* MAE = Mean Absolute Error
 * MSE = Mean Squared Error
 */
enum LOSS_TYPE { MAE, MSE, CROSS_ENTROPY };
enum ACTIVATION_TYPE { RELU, SIGMOID, TANH };

// perlu dimasukan ke parameter template karena semua array di dalamnya statis
template <typename FP, size_t inputSize, size_t hidden1Size, size_t hidden2Size,
          size_t outputSize,
          typename = std::enable_if_t<std::is_floating_point_v<FP>>>
class BasicFFN {
  ACTIVATION_TYPE act_t;
  LOSS_TYPE loss_t;
  FP wIn[inputSize][hidden1Size], wHid1[hidden1Size][hidden2Size],
      wHid2[hidden2Size][outputSize];
  FP bIn[hidden1Size], bHid1[hidden2Size], bHid2[outputSize];
  FP toHid1[hidden1Size], toHid2[hidden2Size], out[outputSize];
  FP dOut[outputSize], dHid2[hidden2Size], dHid1[hidden1Size], dIn[inputSize];
  inline static FP epsilon =
      1e-6;  // untuk mencegah dead neuron ketika menggunakan ReLU
  bool xavier = false;
  FP eta = 1e-2;
  // FP = Floating Point @param FP1 current eta/learning rate @param FP2 grad
  FP (*adaptive_eta_func)(FP, FP);

  // setiap layer punya distribusi yang berbeda
  template <size_t inSize, size_t outSize>
  void init_layer(FP k, FP (&w)[inSize][outSize], FP (&b)[outSize]) {
    // setup random
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<FP> dis(0, std::sqrt(k));

    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) w[j][i] = dis(gen);
      b[i] = epsilon;
    }
  }

  /* He Initialization cuma make k = input
   * sedangkan xavier init make k = input + output;
   */
  void init_wb() {
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

  // Activation func
  static FP ReLU(FP x) { return x > 0 ? x : epsilon; }
  static FP ReLU_deriv(FP y) { return y > 0 ? 1 : epsilon; }
  static FP sigmoid(FP x) { return 1 / (1 + std::exp(-x)); }
  static FP sigmoid_deriv(FP y) { return y * (1 - y); }
  static FP tanh(FP x) {
    return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x));
  }
  static FP tanh_deriv(FP y) { return 1 - y * y; }

  /* Agar lebih mudah dibaca, layer layernya diabstraksi jadi fungsi independen.
  Untuk performa sih lebih baik di unwrap karena nambah overhead untuk
  pemanggilan fungsi kalau diabstraksi semua. Lebih bagus lagi kalau di taruh di
  shader compute GPU, atau pake Tensorflownya CUDA sekalian. Next time bakal
  diunwrap
  */
  // fungsi untuk forward per layer, @actFunc pointer fungsi aktivasi atau 0
  // alias nullptr jika tidak
  template <size_t inSize, size_t outSize>
  void forward_layer(FP (&w)[inSize][outSize], FP (&b)[outSize],
                     FP (&dataIn)[inSize], FP (&dataOut)[outSize],
                     FP (*actFunc)(FP)) {
    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) dataOut[i] += w[j][i] * dataIn[j];
      if (actFunc) dataOut[i] = actFunc(dataOut[i]);
      dataOut[i] += b[i];
    }
  }

  /* fungsi untuk backward per layer
  @param inSize parameter template untuk ukuran layer masuk (dari belakang)
  @param outSize parameter template untuk ukuran layer keluar (dari belakang)
  @param actFuncDeriv pointer ke turunan fungsi aktivasi
   */
  template <size_t inSize, size_t outSize>
  void backward_layer(FP (&w)[outSize][inSize], FP (&b)[inSize],
                      FP (&dataIn)[inSize], FP (&deltaIn)[inSize],
                      FP (&deltaOut)[outSize], FP (*actFuncDeriv)(FP), FP eta) {
    // update bobot dan bias berdasarkan delta in
    for (size_t i = 0; i < inSize; ++i) {
      for (size_t j = 0; j < outSize; ++j)
        w[j][i] -= eta * deltaIn[inSize] * dataIn[i];
      b[i] -= eta * deltaIn[inSize];
    }

    // update delta Outnya (ga berguna buat layer hidden ke input)
    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) deltaOut[i] += w[i][j] * deltaIn[j];
      // aturan rantai
      deltaOut[i] *= actFuncDeriv(dataIn[i]);
    }
  }

 public:
  // default activation using ReLU
  explicit BasicFFN(ACTIVATION_TYPE act_t = ACTIVATION_TYPE::RELU,
                    LOSS_TYPE loss_t = LOSS_TYPE::MSE)
      : act_t(act_t), loss_t(loss_t) {
    init_wb();
  }

  // set epsilon if using ReLU to avoid dead neuron, @param epsilon default 1e-6
  void set_epsilon(FP epsilon) { BasicFFN::epsilon = epsilon; }
  void set_learning_rate(FP eta) { this->eta = eta; }
  void set_adaptive_learning_rate_func(FP (*adaptive_eta_func)(FP, FP)) {
    this->adaptive_eta_func = adaptive_eta_func;
  }

  /* return output in array
  @note array will be lost after class is destroyed
  */
  FP (&forward(FP (&data)[inputSize]))[outputSize] {
    auto actFuncFromType = [](ACTIVATION_TYPE act_t) {
      switch (act_t) {
        case ACTIVATION_TYPE::RELU: return ReLU;
        case ACTIVATION_TYPE::SIGMOID: return sigmoid;
        case ACTIVATION_TYPE::TANH: return tanh;
      }
    };
    auto actFunc = actFuncFromType(act_t);

    // input data to the input layer
    forward_layer<inputSize, hidden1Size>(wIn, bIn, data, toHid1, 0);
    // hidden layer
    forward_layer<hidden1Size, hidden2Size>(wHid1, bHid1, toHid1, toHid2,
                                            actFunc);
    forward_layer<hidden2Size, outputSize>(wHid2, bHid2, toHid2, out, actFunc);

    return out;
  }

  // Loss function section
  // Mean Absolute Error
  static FP MAE(FP ypred, FP y) { return std::abs(ypred - y); }
  // Derivative of Mean Absolute Error
  static FP MAE_deriv(FP ypred, FP y) {
    return (ypred > y) ? 1 : (ypred < y) ? -1 : 0;
  }
  // Mean Squared Error
  static FP MSE(FP ypred, FP y) {
    FP semi_loss = ypred - y;
    return 0.5 * semi_loss * semi_loss;
  }
  // Derivative of Mean Squared Error
  static FP MSE_deriv(FP ypred, FP y) { return ypred - y; }

  static FP cross_entropy_loss(FP ypred, FP y) {
    return -y * std::log(ypred + epsilon) -
           (1 - y) * std::log(1 - ypred + epsilon);
  }
  static FP cross_entropy_loss_deriv(FP ypred, FP y) {
    return (ypred - y) / ((ypred + epsilon) * (1 - ypred + epsilon));
  }

  /* using w = wcurr - eta * dL/dw
   using b = bcurr - eta * dL/db;
   */
  void backward(FP (&inputData)[inputSize], FP (&targetData)[outputSize]) {
    forward(inputData);
    auto actFuncDerivFromType = [](ACTIVATION_TYPE act_t) {
      switch (act_t) {
        case ACTIVATION_TYPE::RELU: return ReLU_deriv;
        case ACTIVATION_TYPE::SIGMOID: return sigmoid_deriv;
        case ACTIVATION_TYPE::TANH: return tanh_deriv;
      }
    };
    auto actFuncDeriv = actFuncDerivFromType(act_t);

    auto lossDerivFuncFromType = [](LOSS_TYPE loss_t) {
      switch (loss_t) {
        case LOSS_TYPE::MAE: return MAE_deriv;
        case LOSS_TYPE::MSE: return MSE_deriv;
        case LOSS_TYPE::CROSS_ENTROPY: return cross_entropy_loss_deriv;
      }
    };
    auto lossDerivFunc = lossDerivFuncFromType(loss_t);

    // calculate dOut first for update through backward_layer template function
    for (size_t i = 0; i < outputSize; ++i)
      dOut[i] = lossDerivFunc(out[i], targetData[i]) * actFuncDeriv(out[i]);

    backward_layer<outputSize, hidden2Size>(wHid2, bHid2, dOut, dHid2,
                                            actFuncDeriv, eta);
    backward_layer<hidden2Size, hidden1Size>(wHid1, bHid1, dHid2, dHid1,
                                             actFuncDeriv, eta);
    backward_layer<hidden1Size, inputSize>(wIn, bIn, dHid1, dIn, actFuncDeriv,
                                           eta);
  }
};

}  // namespace NN
