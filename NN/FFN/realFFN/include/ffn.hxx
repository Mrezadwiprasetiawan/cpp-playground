#pragma once

#include <nn_handler.hxx>
#include <nn_objects.hxx>
#include <random>
#include <type_traits>
#include <vector>

namespace NN {
TEMPLATE_FLOAT
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