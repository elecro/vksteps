#pragma once

#include <vulkan/vulkan_core.h>

#include "context.h"

struct GLFWwindow;

class IMGUIIntegration {
public:
    IMGUIIntegration() {}

    bool Init(GLFWwindow* window);
    bool CreateContext(const Context& context, const VkFormat swapchainFormat);
    void NewFrame();
    void Draw(const VkCommandBuffer cmdBuffer);

    void Destroy(const Context& context);

    bool WantCaptureKeyboard();
protected:
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
};
