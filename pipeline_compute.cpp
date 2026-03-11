#include "pipeline_compute.h"

#include <cassert>

#include "debug.h"
#include "wrappers.h"

namespace {

#include "pipeline_compute.comp_include.h"

} // namespace

void PipelineComputePos::Create(const VkDevice device)
{
    // Layout: nothing used!
    m_layout = CreatePipelineLayout(device, {}, sizeof(ComputePushConstant), "computeLayout");

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        .codeSize = sizeof(SPV_compute_comp),
        .pCode    = SPV_compute_comp,
    };

    VkComputePipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage =
            VkPipelineShaderStageCreateInfo{
                .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext               = &shaderModuleCreateInfo,
                .flags               = 0,
                .stage               = VK_SHADER_STAGE_COMPUTE_BIT,
                .module              = VK_NULL_HANDLE,
                .pName               = "main",
                .pSpecializationInfo = nullptr,
            },
        .layout             = m_layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = 0,
    };

    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &m_pipeline);
    SetResourceName(device, VK_OBJECT_TYPE_PIPELINE, m_pipeline, "simpleComputePipeline");
}

void PipelineComputePos::SetBuffers(VkDevice device, BufferInfo& src, BufferInfo& dst)
{
    m_pushData.srcAddress = src.address;
    m_pushData.dstAddress = dst.address;
}

void PipelineComputePos::CmdDispatch(VkCommandBuffer cmdBuffer)
{
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
    vkCmdPushConstants(cmdBuffer, m_layout, VK_SHADER_STAGE_ALL, 0, sizeof(m_pushData), &m_pushData);
    vkCmdDispatch(cmdBuffer, 1, 1, 1);
}

void PipelineComputePos::Destroy(const VkDevice device)
{
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_layout, nullptr);
}
