#pragma once

#include <vector>

#include "nn_objects.hxx"

// currently just a placeholder
namespace NN {
enum GPU_MODE { vulkan = 1, cuda = 2, opencl = 4, metal = 8, NONE = 0 };
TEMPLATE_FLOAT
class NNHandler {
  Layer layer;
  std::string shader_compute;
  COMPUTE_MODE mode;
  int gpu_mode;

  // check availabily driver for the api, unimplemented yet
  int validate_gpu_mode() const { return GPU_MODE::NONE; }

 public:
  explicit NNHandler(COMPUTE_MODE mode) : mode(mode) {
    if (mode == COMPUTE_CPU) return;
    gpu_mode = validate_gpu_mode();
    assert(gpu_mode != GPU_MODE::NONE && "No GPU mode available, please check your drivers or use CPU mode.");
  }
  NNHandler() = delete;

  GPU_MODE get_gpu_mode() const {
    if (mode == COMPUTE_MODE::CPU) return GPU_MODE::NONE;
    return static_cast<GPU_MODE>(gpu_mode);
  }
  void set_shader_compute(const std::string& shader) { shader_compute = shader; }
  void set_layer(const Layer& l) { layer = l; }
  bool set_mode(COMPUTE_MODE m) {}

  /* this method is used to set the GPU mode
  if compute mode is CPU, this method is useless
  */
  void set_gpu_mode(GPU_MODE mode) {
    if (mode == COMPUTE_MODE::CPU || !(get_gpu_mode() & mode)) return;
    gpu_mode = mode;
  }
  // run compute for each layer, unimplemented yet
  void run(Layer prevLayer, Layer<FP> nextLayer) {}
};
}  // namespace NN