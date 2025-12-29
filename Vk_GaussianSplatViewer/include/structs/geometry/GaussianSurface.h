#pragma once

#include <array>
#include <vulkan/vulkan_core.h>

struct GaussianSurface
{
    float position[3];
    float normal[3];

    float f_dc[3];        // f_dc_0 .. f_dc_2
    float f_rest[45];     // f_rest_0 .. f_rest_44

    float opacity;

    float scale[3];       // scale_0 .. scale_2
    float rotation[4];    // rot_0 .. rot_3
};

struct GaussianSurfaceDescriptor
{
    static VkVertexInputBindingDescription2EXT get_binding_description()
    {
        VkVertexInputBindingDescription2EXT binding_description{};
        binding_description.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
        binding_description.pNext = nullptr;
        binding_description.binding = 0;
        binding_description.stride = sizeof(GaussianSurface);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        binding_description.divisor = 1;

        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription2EXT, 6> get_attribute_descriptions()
    {
        std::array<VkVertexInputAttributeDescription2EXT, 6> attributes{};

        // position (vec3)
        attributes[0].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
        attributes[0].pNext = nullptr;
        attributes[0].location = 0;
        attributes[0].binding = 0;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset = offsetof(GaussianSurface, position);

        // normal (vec3)
        attributes[1].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
        attributes[1].pNext = nullptr;
        attributes[1].location = 1;
        attributes[1].binding = 0;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(GaussianSurface, normal);

        // f_dc (vec3)
        attributes[2].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
        attributes[2].pNext = nullptr;
        attributes[2].location = 2;
        attributes[2].binding = 0;
        attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[2].offset = offsetof(GaussianSurface, f_dc);

        // opacity (float)
        attributes[3].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
        attributes[3].pNext = nullptr;
        attributes[3].location = 3;
        attributes[3].binding = 0;
        attributes[3].format = VK_FORMAT_R32_SFLOAT;
        attributes[3].offset = offsetof(GaussianSurface, opacity);

        // scale (vec3)
        attributes[4].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
        attributes[4].pNext = nullptr;
        attributes[4].location = 4;
        attributes[4].binding = 0;
        attributes[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[4].offset = offsetof(GaussianSurface, scale);

        // rotation (vec4)
        attributes[5].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
        attributes[5].pNext = nullptr;
        attributes[5].location = 5;
        attributes[5].binding = 0;
        attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[5].offset = offsetof(GaussianSurface, rotation);

        return attributes;
    }
};

