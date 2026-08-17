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
#include <array>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "nn_objects.hxx"

namespace NN {

enum GPU_MODE { cuda = 1, vulkan = 2, opencl = 4, metal = 8, NONE = 0 };

template <std::floating_point FP, size_t InSize, size_t OutSize>
class NNHandler {
  const std::string engineName = "NO ENGINE";
  const std::string appName    = "Neural Network Compute Layer";

  Layer<FP, InSize, OutSize> layer;
  COMPUTE_MODE                mode;
  int                         gpu_mode = GPU_MODE::NONE;

  vk::raii::Context                        context;
  std::optional<vk::raii::Instance>        instance;
  std::optional<vk::raii::PhysicalDevice>  physicalDevice;
  std::optional<vk::raii::Device>          device;
  std::optional<vk::raii::Queue>           computeQueue;
  uint32_t                                 computeQueueFamilyIndex = 0;

  std::optional<vk::raii::CommandPool>   commandPool;
  std::optional<vk::raii::CommandBuffer> commandBuffer;

  std::optional<vk::raii::ShaderModule>        shaderModule;
  std::optional<vk::raii::DescriptorSetLayout> descriptorSetLayout;
  std::optional<vk::raii::PipelineLayout>      pipelineLayout;
  std::optional<vk::raii::Pipeline>            computePipeline;
  std::optional<vk::raii::DescriptorPool>      descriptorPool;
  std::optional<vk::raii::DescriptorSet>       descriptorSet;
  std::optional<vk::raii::Fence>               computeFence;

  std::optional<vk::raii::Buffer>       wBuffer, bBuffer, xBuffer, yBuffer;
  std::optional<vk::raii::DeviceMemory> wMemory, bMemory, xMemory, yMemory;
  void *wMapped = nullptr, *bMapped = nullptr, *xMapped = nullptr, *yMapped = nullptr;

  bool            gpuReady           = false;
  ACTIVATION_TYPE compiledActivation = ACTIVATION_TYPE::NONE;

  bool check_vulkan_available() const {
    try {
      vk::raii::Context ctx;
      (void)ctx.enumerateInstanceExtensionProperties();
      return true;
    } catch (...) {
      return false;
    }
  }

  int validate_gpu_mode() const {
    int res = 0;
    if (check_vulkan_available()) res |= GPU_MODE::vulkan;
    return res;
  }

  uint32_t find_memory_type(uint32_t typeFilter, vk::MemoryPropertyFlags props) const {
    vk::PhysicalDeviceMemoryProperties memProps = physicalDevice->getMemoryProperties();
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
      if ((typeFilter & (1u << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props) return i;
    }
    throw std::runtime_error("no suitable vulkan memory type found");
  }

  std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage) {
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);
    vk::raii::Buffer      buffer = device->createBuffer(bufferInfo);

    vk::MemoryRequirements memReq = buffer.getMemoryRequirements();
    uint32_t                memType =
        find_memory_type(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    vk::MemoryAllocateInfo allocInfo(memReq.size, memType);
    vk::raii::DeviceMemory  memory = device->allocateMemory(allocInfo);
    buffer.bindMemory(*memory, 0);

    return {std::move(buffer), std::move(memory)};
  }

  void ensure_shader_module() {
    if (shaderModule) return;

    std::ostringstream src;
    src << "#version 450\n"
           "layout(local_size_x = 64) in;\n"
           "layout(constant_id = 0) const uint IN_SIZE = 1;\n"
           "layout(constant_id = 1) const uint OUT_SIZE = 1;\n"
           "layout(constant_id = 2) const uint ACT_TYPE = 3;\n"
           "layout(set = 0, binding = 0) readonly buffer WBuf { float w[]; };\n"
           "layout(set = 0, binding = 1) readonly buffer BBuf { float b[]; };\n"
           "layout(set = 0, binding = 2) readonly buffer XBuf { float x[]; };\n"
           "layout(set = 0, binding = 3) writeonly buffer YBuf { float y[]; };\n"
           "float activate(float v) {\n"
           "  if (ACT_TYPE == 0u) return 1.0 / (1.0 + exp(-v));\n"
           "  if (ACT_TYPE == 1u) return v > 0.0 ? v : 1e-6;\n"
           "  if (ACT_TYPE == 2u) return tanh(v);\n"
           "  return v;\n"
           "}\n"
           "void main() {\n"
           "  uint i = gl_GlobalInvocationID.x;\n"
           "  if (i >= OUT_SIZE) return;\n"
           "  float sum = b[i];\n"
           "  for (uint j = 0u; j < IN_SIZE; ++j) sum += w[i * IN_SIZE + j] * x[j];\n"
           "  y[i] = activate(sum);\n"
           "}\n";

    std::string tmpGlsl = "/tmp/nnhandler_" + std::to_string(reinterpret_cast<uintptr_t>(this)) + ".comp";
    std::string tmpSpv  = tmpGlsl + ".spv";

    std::ofstream ofs(tmpGlsl);
    ofs << src.str();
    ofs.close();

    std::string cmd = "glslangValidator -V " + tmpGlsl + " -o " + tmpSpv + " > /dev/null 2>&1";
    int         ret = std::system(cmd.c_str());
    if (ret != 0) throw std::runtime_error("failed to compile compute shader with glslangValidator");

    std::ifstream spvFile(tmpSpv, std::ios::binary | std::ios::ate);
    size_t        spvSize = static_cast<size_t>(spvFile.tellg());
    std::vector<uint32_t> spvCode(spvSize / sizeof(uint32_t));
    spvFile.seekg(0);
    spvFile.read(reinterpret_cast<char *>(spvCode.data()), static_cast<std::streamsize>(spvSize));
    spvFile.close();

    std::remove(tmpGlsl.c_str());
    std::remove(tmpSpv.c_str());

    vk::ShaderModuleCreateInfo shaderInfo({}, spvCode.size() * sizeof(uint32_t), spvCode.data());
    shaderModule.emplace(device->createShaderModule(shaderInfo));
  }

  void build_pipeline() {
    ensure_shader_module();

    std::array<uint32_t, 3> specData{static_cast<uint32_t>(InSize), static_cast<uint32_t>(OutSize), static_cast<uint32_t>(layer.act_func_t)};
    std::array<vk::SpecializationMapEntry, 3> specEntries{
        vk::SpecializationMapEntry(0, 0 * sizeof(uint32_t), sizeof(uint32_t)),
        vk::SpecializationMapEntry(1, 1 * sizeof(uint32_t), sizeof(uint32_t)),
        vk::SpecializationMapEntry(2, 2 * sizeof(uint32_t), sizeof(uint32_t)),
    };
    vk::SpecializationInfo specInfo(static_cast<uint32_t>(specEntries.size()), specEntries.data(), specData.size() * sizeof(uint32_t), specData.data());

    vk::PipelineShaderStageCreateInfo stageInfo({}, vk::ShaderStageFlagBits::eCompute, **shaderModule, "main", &specInfo);
    vk::ComputePipelineCreateInfo     pipelineInfo({}, stageInfo, **pipelineLayout);

    computePipeline.reset();
    computePipeline.emplace(device->createComputePipeline(nullptr, pipelineInfo));
    compiledActivation = layer.act_func_t;
  }

  void init_vulkan() {
    if (gpuReady) return;

    const uint32_t       apiVers = context.enumerateInstanceVersion();
    vk::ApplicationInfo  appInfo(appName.c_str(), 1, engineName.c_str(), 1, apiVers);
    vk::InstanceCreateInfo instInfo({}, &appInfo);
    instance.emplace(context.createInstance(instInfo));

    std::vector<vk::raii::PhysicalDevice> devices = instance->enumeratePhysicalDevices();
    assert(!devices.empty());
    physicalDevice.emplace(std::move(devices[0]));

    std::vector<vk::QueueFamilyProperties> queueProps = physicalDevice->getQueueFamilyProperties();
    auto                                   it = std::find_if(queueProps.begin(), queueProps.end(), [](const vk::QueueFamilyProperties &p) {
      return static_cast<bool>(p.queueFlags & vk::QueueFlagBits::eCompute);
    });
    assert(it != queueProps.end());
    computeQueueFamilyIndex = static_cast<uint32_t>(std::distance(queueProps.begin(), it));

    const float                  priority = 1.0f;
    vk::DeviceQueueCreateInfo queueInfo({}, computeQueueFamilyIndex, 1, &priority);
    vk::DeviceCreateInfo      deviceInfo({}, queueInfo);
    device.emplace(physicalDevice->createDevice(deviceInfo));
    computeQueue.emplace(device->getQueue(computeQueueFamilyIndex, 0));

    vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, computeQueueFamilyIndex);
    commandPool.emplace(device->createCommandPool(poolInfo));

    vk::CommandBufferAllocateInfo        cmdAllocInfo(**commandPool, vk::CommandBufferLevel::ePrimary, 1);
    std::vector<vk::raii::CommandBuffer> cmdBuffers = device->allocateCommandBuffers(cmdAllocInfo);
    commandBuffer.emplace(std::move(cmdBuffers[0]));

    std::array<vk::DescriptorSetLayoutBinding, 4> bindings{
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
    };
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, static_cast<uint32_t>(bindings.size()), bindings.data());
    descriptorSetLayout.emplace(device->createDescriptorSetLayout(layoutInfo));

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1, &**descriptorSetLayout);
    pipelineLayout.emplace(device->createPipelineLayout(pipelineLayoutInfo));

    vk::DescriptorPoolSize       poolSize(vk::DescriptorType::eStorageBuffer, 4);
    vk::DescriptorPoolCreateInfo descPoolInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, 1, &poolSize);
    descriptorPool.emplace(device->createDescriptorPool(descPoolInfo));

    vk::DescriptorSetAllocateInfo        descAllocInfo(**descriptorPool, 1, &**descriptorSetLayout);
    std::vector<vk::raii::DescriptorSet> sets = device->allocateDescriptorSets(descAllocInfo);
    descriptorSet.emplace(std::move(sets[0]));

    auto [wb, wm] = create_buffer(sizeof(FP) * OutSize * InSize, vk::BufferUsageFlagBits::eStorageBuffer);
    wBuffer.emplace(std::move(wb));
    wMemory.emplace(std::move(wm));
    auto [bb, bm] = create_buffer(sizeof(FP) * OutSize, vk::BufferUsageFlagBits::eStorageBuffer);
    bBuffer.emplace(std::move(bb));
    bMemory.emplace(std::move(bm));
    auto [xb, xm] = create_buffer(sizeof(FP) * InSize, vk::BufferUsageFlagBits::eStorageBuffer);
    xBuffer.emplace(std::move(xb));
    xMemory.emplace(std::move(xm));
    auto [yb, ym] = create_buffer(sizeof(FP) * OutSize, vk::BufferUsageFlagBits::eStorageBuffer);
    yBuffer.emplace(std::move(yb));
    yMemory.emplace(std::move(ym));

    wMapped = wMemory->mapMemory(0, sizeof(FP) * OutSize * InSize);
    bMapped = bMemory->mapMemory(0, sizeof(FP) * OutSize);
    xMapped = xMemory->mapMemory(0, sizeof(FP) * InSize);
    yMapped = yMemory->mapMemory(0, sizeof(FP) * OutSize);

    std::array<vk::DescriptorBufferInfo, 4> bufferInfos{
        vk::DescriptorBufferInfo(**wBuffer, 0, VK_WHOLE_SIZE),
        vk::DescriptorBufferInfo(**bBuffer, 0, VK_WHOLE_SIZE),
        vk::DescriptorBufferInfo(**xBuffer, 0, VK_WHOLE_SIZE),
        vk::DescriptorBufferInfo(**yBuffer, 0, VK_WHOLE_SIZE),
    };
    std::array<vk::WriteDescriptorSet, 4> writes{
        vk::WriteDescriptorSet(**descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufferInfos[0]),
        vk::WriteDescriptorSet(**descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufferInfos[1]),
        vk::WriteDescriptorSet(**descriptorSet, 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufferInfos[2]),
        vk::WriteDescriptorSet(**descriptorSet, 3, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufferInfos[3]),
    };
    device->updateDescriptorSets(writes, nullptr);

    vk::FenceCreateInfo fenceInfo;
    computeFence.emplace(device->createFence(fenceInfo));

    build_pipeline();
    gpuReady = true;
  }

  void run_vulkan() {
    init_vulkan();
    if (compiledActivation != layer.act_func_t) build_pipeline();

    std::memcpy(wMapped, layer.w.data(), sizeof(FP) * OutSize * InSize);
    std::memcpy(bMapped, layer.b.data(), sizeof(FP) * OutSize);
    std::memcpy(xMapped, layer.inputs.data(), sizeof(FP) * InSize);

    commandBuffer->reset({});
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer->begin(beginInfo);
    commandBuffer->bindPipeline(vk::PipelineBindPoint::eCompute, **computePipeline);
    commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute, **pipelineLayout, 0, {**descriptorSet}, {});
    uint32_t groups = (static_cast<uint32_t>(OutSize) + 63u) / 64u;
    commandBuffer->dispatch(groups, 1, 1);
    commandBuffer->end();

    device->resetFences({**computeFence});
    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &**commandBuffer);
    computeQueue->submit({submitInfo}, **computeFence);
    vk::Result waitResult = device->waitForFences({**computeFence}, VK_TRUE, UINT64_MAX);
    if (waitResult != vk::Result::eSuccess) throw std::runtime_error("vulkan compute fence wait failed");

    std::memcpy(layer.outputs.data(), yMapped, sizeof(FP) * OutSize);
  }

 public:
  explicit NNHandler(COMPUTE_MODE mode) : mode(mode) {
    if (mode == COMPUTE_MODE::CPU) return;
    gpu_mode = validate_gpu_mode();
    assert(gpu_mode != GPU_MODE::NONE && "No GPU mode available, please check your drivers or use CPU mode.");
  }
  NNHandler() = delete;

  ~NNHandler() {
    if (!gpuReady) return;
    if (wMemory) wMemory->unmapMemory();
    if (bMemory) bMemory->unmapMemory();
    if (xMemory) xMemory->unmapMemory();
    if (yMemory) yMemory->unmapMemory();
  }

  GPU_MODE get_gpu_mode() const {
    if (mode == COMPUTE_MODE::CPU) return GPU_MODE::NONE;
    return static_cast<GPU_MODE>(gpu_mode);
  }

  bool set_mode(COMPUTE_MODE m) {
    if (m == COMPUTE_MODE::GPU && validate_gpu_mode() == GPU_MODE::NONE) return false;
    mode = m;
    if (mode == COMPUTE_MODE::GPU) gpu_mode = validate_gpu_mode();
    return true;
  }

  void set_gpu_mode(GPU_MODE m) {
    if (mode == COMPUTE_MODE::CPU || !(get_gpu_mode() & m)) return;
    gpu_mode = m;
  }

  void set_layer(const Layer<FP, InSize, OutSize> &l) { layer = l; }
  Layer<FP, InSize, OutSize> &get_layer() { return layer; }

  void run() {
    if (mode == COMPUTE_MODE::CPU) {
      layer.outputs = apply_activation(layer.w * layer.inputs + layer.b, layer.act_func_t);
      return;
    }

    if (gpu_mode & GPU_MODE::cuda) return;

    if (gpu_mode & GPU_MODE::vulkan) {
      run_vulkan();
      return;
    }
  }
};
}  // namespace NN
