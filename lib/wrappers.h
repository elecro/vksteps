#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

void PrintPhyDeviceInfo(const VkInstance /*instance*/, const VkPhysicalDevice phyDevice);

VkCommandPool CreateCommandPool(const VkDevice device, const uint32_t queueFamilyIdx);
std::vector<VkCommandBuffer>
AllocateCommandBuffers(const VkDevice device, const VkCommandPool cmdPool, const uint32_t count);

VkFence CreateFence(const VkDevice device);

VkSemaphore CreateSemaphore(const VkDevice device);

VkShaderModule CreateShaderModule(const VkDevice device, const uint32_t* SPIRVBinary, uint32_t SPIRVBinarySize);

VkPipelineLayout CreatePipelineLayout(const VkDevice                            device,
                                      const std::vector<VkDescriptorSetLayout>& layouts,
                                      uint32_t                                  pushConstantSize = 0);
