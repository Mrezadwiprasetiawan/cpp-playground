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
#include <type_traits>
namespace NN {

enum LOSS_TYPE {
  MAE,
  MSE,
};
enum ACTIVATION_TYPE { RELU, SIGMOID, TANH };

// peelu dimasukan ke typename karena semua array di dalamnya statis
template <typename FP, size_t inputSize, size_t hidden1Size, size_t hidden2Size,
          size_t outputSize,
          typename = std::enable_if_t<std::is_floating_point_v<FP>, FP>>
class FFN {
  ACTIVATION_TYPE act_t;
  FP wIn[inputSize][hidden1Size], wHid1[hidden1Size][hidden2Size],
      wHid2[hidden2Size][outputSize];
  FP bIn[hidden1Size], bHid1[hidden2Size], bHid2[outputSize];
  FP resIn[hidden1Size], resHid1[hidden2Size], res[outputSize];
  bool xavier = false;

  template <size_t inSize, size_t outSize>
  void init_layer(FP k, FP (&w)[inSize][outSize], FP (&b)[inSize]) {
    std::random_device rd;
    std::mt19937 gen(rd);
    std::normal_distribution<FP> dis(0, std::sqrt(k));

    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j) w[j][i] = dis(gen);
      b[i] = 1e-6;
    }
  }

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

  static FP ReLU(FP x) { return x > 0 ? x : 1e-6; }
  static FP ReLU_deriv(FP y) { return y > 0 ? 1 : 1e-6; }
  static FP sigmoid(FP x) { return 1 / (1 + std::exp(-x)); }
  static FP sigmoid_deriv(FP y) { return y * (1 - y); }
  static FP tanh(FP x) {
    return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x));
  }
  static FP tanh_deriv(FP y) { return 1 - y * y; }

  template <size_t inSize, size_t outSize, bool hidden>
  void forwardlayer(FP (&w)[inSize][outSize], FP (&b)[outSize],
                    FP (&in)[inputSize], FP (&res)[outSize],
                    FP (*actFunc)(FP x)) {
    for (size_t i = 0; i < outSize; ++i) {
      for (size_t j = 0; j < inSize; ++j)
        res[i] += hidden ? actFunc(in[j] * w[j][i]) : w[j][i] * in[j];
      res[i] += b[i];
    }
  }

 public:
  FFN(ACTIVATION_TYPE act_t = ACTIVATION_TYPE::RELU) : act_t(act_t) {
    init_wb();
  }
  FP (&forward(FP (&data)[inputSize]))[outputSize] {
    // reset layer data first
    resIn = {};
    resHid1 = {};
    res = {};
    auto actFuncFromType = [](ACTIVATION_TYPE act_t) {
      switch (act_t) {
        case ACTIVATION_TYPE::RELU: return ReLU;
        case ACTIVATION_TYPE::SIGMOID: return sigmoid;
        case ACTIVATION_TYPE::TANH: return tanh;
      }
    };
    forwardlayer<inputSize, outputSize, 0>(wIn, bIn, data, resIn, 0);
    forwardlayer<outputSize, hidden1Size, 1>(wHid1, bHid1, resIn, resHid1,
                                             actFuncFromType(act_t));
    forwardlayer<hidden1Size, hidden2Size>(wHid2, bHid2, resHid1, res,
                                           actFuncFromType(act_t));
    return res;
  }

  void backward(FP (&data)[outputSize]) {
    auto actDerivFuncFromType = [](ACTIVATION_TYPE act_t) {
      switch (act_t) {
        case ACTIVATION_TYPE::RELU: return ReLU_deriv;
        case ACTIVATION_TYPE::SIGMOID: return sigmoid_deriv;
        case ACTIVATION_TYPE::TANH: return tanh_deriv;
      }
    };
  }
};

}  // namespace NN
