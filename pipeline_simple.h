#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

class PipelineSimple {
public:
    PipelineSimple(const std::string& name)
        : m_name(name)
    {
    }

    void Create(const VkDevice device, const VkFormat colorFormat);
    void Destroy(const VkDevice device);

    VkPipeline       pipeline() const { return m_pipeline; }
    VkPipelineLayout layout() const { return m_layout; }

private:
    VkPipeline CreatePipeline(const VkDevice       device,
                              const VkShaderModule shaderVertex,
                              const VkShaderModule shaderFragment,
                              const VkFormat       colorFormat);

    const std::string m_name;
    VkPipeline        m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout  m_layout   = VK_NULL_HANDLE;
};
