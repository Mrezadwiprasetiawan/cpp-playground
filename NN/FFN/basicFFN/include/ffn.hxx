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
