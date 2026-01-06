#pragma once

#include <vulkan/vulkan_core.h>

struct PushConstantBlock
{
    VkDeviceAddress scene_buffer_address;
    VkDeviceAddress positions_buffer_address;
    VkDeviceAddress scales_buffer_address;
    VkDeviceAddress colors_buffer_address;
    VkDeviceAddress quaternions_buffer_address;
    VkDeviceAddress alpha_buffer_address;
};