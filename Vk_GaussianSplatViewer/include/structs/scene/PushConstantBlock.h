#pragma once

#include <vulkan/vulkan_core.h>

struct PushConstantBlock
{
    VkDeviceAddress scene_buffer_address;
};