#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

void PrintPhyDeviceInfo(const VkInstance /*instance*/, const VkPhysicalDevice phyDevice);

VkCommandPool CreateCommandPool(const VkDevice device, const uint32_t queueFamilyIdx, const std::string& name = "");
std::vector<VkCommandBuffer> AllocateCommandBuffers(const VkDevice      device,
                                                    const VkCommandPool cmdPool,
                                                    const uint32_t      count,
                                                    const std::string&  name = "");

VkFence CreateFence(const VkDevice device, const std::string& name = "");

VkSemaphore CreateSemaphore(const VkDevice device, const std::string& name = "");

VkShaderModule CreateShaderModule(const VkDevice     device,
                                  const uint32_t*    SPIRVBinary,
                                  uint32_t           SPIRVBinarySize,
                                  const std::string& name = "");

VkPipelineLayout CreatePipelineLayout(const VkDevice                            device,
                                      const std::vector<VkDescriptorSetLayout>& layouts,
                                      uint32_t                                  pushConstantSize = 0,
                                      const std::string&                        name             = "");
