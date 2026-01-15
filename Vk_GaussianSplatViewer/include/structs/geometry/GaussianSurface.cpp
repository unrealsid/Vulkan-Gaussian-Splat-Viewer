#include "GaussianSurface.h"
#include <glm/glm.hpp>

std::array<VkVertexInputBindingDescription2EXT, 2> GaussianSurfaceDescriptor::get_binding_descriptions()
{
    std::array<VkVertexInputBindingDescription2EXT, 2> bindings{};

    // Binding 0: positions (vec3)
    bindings[0].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
    bindings[0].pNext = nullptr;
    bindings[0].binding = 0;
    bindings[0].stride = sizeof(glm::vec4);
    bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings[0].divisor = 1;

    // Binding 1: colors (vec4)
    bindings[1].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
    bindings[1].pNext = nullptr;
    bindings[1].binding = 1;
    bindings[1].stride = sizeof(glm::vec4);
    bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings[1].divisor = 1;

    return bindings;
}

std::array<VkVertexInputAttributeDescription2EXT, 2> GaussianSurfaceDescriptor::get_attribute_descriptions()
{
    std::array<VkVertexInputAttributeDescription2EXT, 2> attributes{};

    // Location 0: position (vec4 from binding 0)
    attributes[0].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attributes[0].pNext = nullptr;
    attributes[0].location = 0;
    attributes[0].binding = 0;
    attributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[0].offset = 0;

    // Location 0: color (vec4 from binding 0)
    attributes[1].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attributes[1].pNext = nullptr;
    attributes[1].location = 1;
    attributes[1].binding = 1;
    attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[1].offset = 0;

    return attributes;
}
