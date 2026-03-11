#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "descriptors.h"

class PipelineComputePos {
public:
    PipelineComputePos(const std::string& name)
        : m_name(name)
    {
    }

    void Create(const VkDevice device);
    void Destroy(const VkDevice device);

    void SetBuffers(VkDevice device, BufferInfo& src, BufferInfo& dst);

    VkPipeline       pipeline() const { return m_pipeline; }
    VkPipelineLayout layout() const { return m_layout; }


private:
    const std::string m_name;
    VkPipeline        m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout  m_layout   = VK_NULL_HANDLE;

    DescriptorMgmt    m_descMgmt;
};
