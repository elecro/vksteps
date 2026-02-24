#include "wrappers.h"

#include <cassert>
#include <cstdio>
#include <vulkan/vulkan_core.h>

void PrintPhyDeviceInfo(const VkInstance /*instance*/, const VkPhysicalDevice phyDevice)
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(phyDevice, &properties);

    uint32_t apiMajor = VK_API_VERSION_MAJOR(properties.apiVersion);
    uint32_t apiMinor = VK_API_VERSION_MINOR(properties.apiVersion);

    printf("Device Info: %s Vulkan API Version: %u.%u\n", properties.deviceName, apiMajor, apiMinor);
}

VkCommandPool CreateCommandPool(const VkDevice device, const uint32_t queueFamilyIdx)
{
    VkCommandPoolCreateInfo createInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIdx,
    };

    VkCommandPool cmdPool = VK_NULL_HANDLE;
    VkResult result = vkCreateCommandPool(device, &createInfo, nullptr, &cmdPool);
    assert((result == VK_SUCCESS) && "Failed to Create VkCommandPool");

    return cmdPool;
}

std::vector<VkCommandBuffer>
AllocateCommandBuffers(const VkDevice device, const VkCommandPool cmdPool, const uint32_t count)
{
    std::vector<VkCommandBuffer> cmdBuffers(count, VK_NULL_HANDLE);

    VkCommandBufferAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = cmdPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t)cmdBuffers.size(),
    };

    // TODO: check result
    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, cmdBuffers.data());
    assert((result == VK_SUCCESS) && "Failed to Create VkCommandBuffers");

    return cmdBuffers;
}

VkFence CreateFence(const VkDevice device)
{
    VkFenceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = 0,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkFence  fence  = VK_NULL_HANDLE;
    VkResult result = vkCreateFence(device, &createInfo, nullptr, &fence);
    assert((result == VK_SUCCESS) && "Failed to create VkFence");

    return fence;
}

VkSemaphore CreateSemaphore(const VkDevice device)
{
    VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };

    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkResult    result    = vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
    assert((result == VK_SUCCESS) && "Failed to Create VkSemaphore");

    return semaphore;
}

VkShaderModule CreateShaderModule(const VkDevice device, const uint32_t* SPIRVBinary, uint32_t SPIRVBinarySize)
{
    // SPIRVBinarySize is in bytes

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        .codeSize = SPIRVBinarySize,
        .pCode    = SPIRVBinary,
    };

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VkResult       result       = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
    assert((result == VK_SUCCESS) && "Failed to Create VkShaderModule");
    // TODO: result check
    return shaderModule;
}

VkPipelineLayout CreatePipelineLayout(const VkDevice                            device,
                                      const std::vector<VkDescriptorSetLayout>& layouts,
                                      uint32_t                                  pushConstantSize)
{
    const VkPushConstantRange pushConstantRange = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset     = 0,
        .size       = pushConstantSize,
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = (uint32_t)layouts.size(),
        .pSetLayouts            = layouts.data(),
        .pushConstantRangeCount = (pushConstantSize > 0) ? 1u : 0u,
        .pPushConstantRanges    = &pushConstantRange,
    };

    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkResult         result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &layout);
    assert(result == VK_SUCCESS);

    return layout;
}
