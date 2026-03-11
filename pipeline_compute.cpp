#include "pipeline_compute.h"

#include <cassert>

#include "debug.h"
#include "wrappers.h"

#include "glm_config.h"

namespace {

#include "pipeline_compute.comp_include.h"

} // namespace

void PipelineComputePos::Create(const VkDevice device)
{
    m_descMgmt.SetDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    m_descMgmt.SetDescriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    m_descMgmt.CreateLayout(device);
    SetResourceName(device, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, m_descMgmt.Layout(), "computeDescLayout");

    m_descMgmt.CreatePool(device);
    m_descMgmt.CreateDescriptorSets(device, 1);
    SetResourceName(device, VK_OBJECT_TYPE_DESCRIPTOR_SET, m_descMgmt.Set(0).Get(), "computeDescSet");

    // Layout: nothing used!
    m_layout = CreatePipelineLayout(device, {m_descMgmt.Layout()}, 0, "computeLayout");

    // Create Pipeline
    // VkShaderModule compute  = CreateShaderModule(device, SPV_compute_comp, sizeof(SPV_compute_comp),
    // "simpleCompute");

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
    VkDescriptorSet descSet = m_descMgmt.Set(0).Get();

    VkDescriptorBufferInfo srcInfo = {
        .buffer = src.buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };
    VkDescriptorBufferInfo dstInfo = {
        .buffer = dst.buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    VkWriteDescriptorSet writeInfos[2] = {
        {
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = descSet,
            .dstBinding       = 0,
            .dstArrayElement  = 0,
            .descriptorCount  = 1,
            .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &srcInfo,
            .pTexelBufferView = nullptr,
        },
        {
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = descSet,
            .dstBinding       = 1,
            .dstArrayElement  = 0,
            .descriptorCount  = 1,
            .descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &dstInfo,
            .pTexelBufferView = nullptr,
        },
    };

    vkUpdateDescriptorSets(device, 2, writeInfos, 0, nullptr);
}

void PipelineComputePos::Destroy(const VkDevice device)
{
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_layout, nullptr);
    m_descMgmt.Destroy(device);
}
