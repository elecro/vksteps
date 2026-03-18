#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "descriptors.h"
#include "glm_config.h"

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
    VkDescriptorSet  descSet() { return m_descMgmt.Set(0).Get(); }

    void SetBuffer(const VkDevice device, BufferInfo src);

private:
    VkPipeline CreatePipeline(const VkDevice       device,
                              const VkShaderModule shaderVertex,
                              const VkShaderModule shaderFragment,
                              const VkFormat       colorFormat);

    const std::string m_name;
    VkPipeline        m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout  m_layout   = VK_NULL_HANDLE;
    DescriptorMgmt    m_descMgmt;
};
