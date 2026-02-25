#pragma once

#include <string>

#include <vulkan/vulkan_core.h>

template <typename T>
void SetResourceName(const VkDevice device, VkObjectType objectType, const T resource, const std::string& name)
{
    static PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT =
        reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT"));
    if (vkSetDebugUtilsObjectNameEXT == nullptr || name.empty()) {
        return;
    }

    VkDebugUtilsObjectNameInfoEXT info = {
        .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext        = nullptr,
        .objectType   = objectType,
        .objectHandle = reinterpret_cast<uint64_t>(resource),
        .pObjectName  = name.c_str(),
    };

    vkSetDebugUtilsObjectNameEXT(device, &info);
}
