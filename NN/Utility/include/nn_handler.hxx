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