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

#include <algorithm>
#include <cassert>
#include <concepts>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "nn_objects.hxx"

// currently just a placeholder
namespace NN {
enum GPU_MODE { cuda = 1, vulkan = 2, opencl = 4, metal = 8, NONE = 0 };
template <std::floating_point FP>
class NNHandler {
  inline const std::string engineName = "NO ENGINE", appName = "Neural Network Compute Layer";
  Layer<FP>                layer;
  std::string              compute_shader;
  uint32_t                *vulkan_spir_v;
  COMPUTE_MODE             mode;
  int                      gpu_mode;

  // vulkan section
  vk::raii::Context                               context;
  std::optional<vk::raii::Instance>               instance;
  std::optional<vk::raii::DebugUtilsMessengerEXT> debugMessenger;

  vk::PhysicalDevice              physicalDevice{nullptr};
  std::optional<vk::raii::Device> device;
  vk::Queue                       computeQueue{nullptr};
  uint32_t                        computeQueueFamilyIndex = 0;

  std::optional<vk::raii::CommandPool> commandPool;
  std::vector<vk::CommandBuffer>       commandBuffers;

  std::optional<vk::raii::PipelineLayout> pipelineLayout;
  std::optional<vk::raii::Pipeline>       computePipeline;

  std::optional<vk::raii::DescriptorSetLayout> descriptorSetLayout;
  std::optional<vk::raii::DescriptorPool>      descriptorPool;
  std::vector<vk::DescriptorSet>               descriptorSets;

  std::optional<vk::raii::Buffer>       inputBuffer;
  std::optional<vk::raii::DeviceMemory> inputBufferMemory;

  std::optional<vk::raii::Buffer>       outputBuffer;
  std::optional<vk::raii::DeviceMemory> outputBufferMemory;

  std::optional<vk::raii::Fence> computeFence;

  // check availabily driver for the api, unimplemented yet other than vulkan
  int validate_gpu_mode() const {
    int res = 0;

    // check if vulkan is available
    if (vk::raii::Context::getGlobalContext().isVulkanAvailable()) res |= GPU_MODE::vulkan;

    return res;
  }

  void create_vulkan_shader() {}

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

  void set_layer(const Layer<FP> &layer) { this->layer = layer; }
  bool set_mode(COMPUTE_MODE m) {}

  /* this method is used to set the GPU mode
  if compute mode is CPU, this method is useless
  */
  void set_gpu_mode(GPU_MODE mode) {
    if (mode == COMPUTE_MODE::CPU || !(get_gpu_mode() & mode)) return;
    gpu_mode = mode;
  }

  // run compute for each layer, unimplemented yet
  void run() {
    // if on the cpu mode, just return outputs = w.x + b overloaded by class matrix and custom overload vector
    if (mode == COMPUTE_MODE::CPU) {
      layer.outputs = layer.w * layer.inputs + layer.b;
      return;
    }

    // cuda priority is 1
    if (gpu_mode & GPU_MODE::cuda) {
      // CUDA mode, implement cuda kernel here
      // This is just a placeholder, you need to implement the actual cuda kernel
      // and launch it with the inputs and outputs
      return;
    }

    // vulkan priority is 2 since cuda more powerfull
    if (gpu_mode & GPU_MODE::vulkan) {
      if (!instance.has_value()) {
        // for safety using default available api version
        const uint32_t      apiVers = enumerateInstanceVersion();
        vk::ApplicationInfo appInfo{engineName, 1, appName, apiVers};
        instance.emplace(vk::createInstance(context, appInfo));
      }

      if (!physicalDevice) {
        vector<physicalDevice> physicalDevices = instance->enumeratePhysicalDevices();
        assert(!physicalDevices.empty());
        physicalDevice = physicalDevices[0];
      }

      // create device
      if (!device.has_value()) {
        vector<::QueueFamilyProperties> queueFamProps = physDev.getQueueFamilyProperties();
        auto                            queueFamProp =
            find_if(queueFamProps.begin(), queueFamProps.end(), [](QueueFamilyProperties &qFProp) { return qFProp.queueFlags & ::QueueFlagBits::eCompute; });
        computeQueueFamilyIndex          = std::distance(queueFamProps.begin(), queueFamProp);
        const float           priorities = 1.0;
        DeviceQueueCreateInfo devQueueInfo(DeviceQueueCreateFlags(), computeQueueFamilyIndex, 1, &priorities);
        DeviceCreateInfo      devInfo(DeviceCreateFlags(), devQueueInfo);
        Device                dev = physDev.createDevice(devInfo);
      }

      // create buffer and fill
      if (!inputBuffer.has_value()) {}
      if (!inputBufferMemory.has_value()) {}
      if (!outputBuffer.has_value()) {}
      if (!outputBufferMemory.has_value()) {}

      std::stringstream shaderstream;

      // unimplemented yet
      shaderstream << "" << std::endl;

      compute_shader = shaderstream.str();
      // call create vulkan shader method
      create_vulkan_shader();

      return;
    }

    // the last doesnt need if for checking
  }
};
}  // namespace NN