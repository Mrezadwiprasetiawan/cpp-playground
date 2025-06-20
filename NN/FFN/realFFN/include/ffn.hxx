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

#include <concepts>
#include <nn_handler.hxx>
#include <nn_objects.hxx>
#include <random>
#include <vector>

namespace NN {
template <std::floating_point FP>
class FFN {
  std::vector<Layer<FP>> layers;
  std::random_device rd;
  NNHandler<FP> handler;
  std::mt19947 gen;

 public:
  FFN(std::vector<Layer> layers, COMPUTE_MODE mode = COMPUTE_MODE::CPU) : layers(layers) { handler = NNHandler<FP>(mode); }
  FFN() = delete;
  Layer<FP> forward(Layer<FP> input) {
    for (size_t i = 0; i < layers.size(); ++i) {
      Layer<FP> &current_layer = layers[i];
      handler.run(input, current_layer);
      input = current_layer;
    }
    return input;
  }
};
}  // namespace NN