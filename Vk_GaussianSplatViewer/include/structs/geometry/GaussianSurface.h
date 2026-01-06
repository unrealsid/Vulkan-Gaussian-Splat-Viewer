#pragma once

#include <array>
#include <vulkan/vulkan.h>

struct GaussianSurfaceDescriptor
{
    static std::array<VkVertexInputBindingDescription2EXT, 1> get_binding_descriptions();

    static std::array<VkVertexInputAttributeDescription2EXT, 1> get_attribute_descriptions();
};

