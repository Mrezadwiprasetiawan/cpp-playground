/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  This project contains various experiments and explorations in C++,
  including topics such as number systems, neural networks, and 
  visualizations of prime number patterns.

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

#include <type_traits>

#define INPUT_SIZE 1
#define HIDDEN1_SIZE 256
#define HIDDEN2_SIZE 256
#define OUTPUT_SIZE 1

namespace NN {

template <typename FP,
          typename = std::enable_if_t<std::is_floating_point_v<FP>, FP>>
class BaseFFN {
  FP wIn[INPUT_SIZE][HIDDEN1_SIZE], wHid1[HIDDEN1_SIZE][HIDDEN2_SIZE],
      wHid2[HIDDEN2_SIZE][OUTPUT_SIZE];
  FP bIn[INPUT_SIZE], bHid1[HIDDEN1_SIZE], b[OUTPUT_SIZE];
  void init_wb() {}

 public:
  BaseFFN() { init_wb(); }
  FP (&forward())[OUTPUT_SIZE] {}
};

}  // namespace NN
