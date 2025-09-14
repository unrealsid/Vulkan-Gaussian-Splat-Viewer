#pragma once

#include <vulkan/vulkan.h>

namespace vulkanapp
{
    class VulkanFeatureActivator
    {
    public:
        static VkPhysicalDeviceDynamicRenderingFeaturesKHR create_dynamic_Rendering_features();
        static VkPhysicalDeviceShaderObjectFeaturesEXT create_shader_object_features();
        static VkPhysicalDeviceBufferDeviceAddressFeatures create_physics_device_buffer_address();
        static VkPhysicalDeviceDescriptorIndexingFeatures create_physical_device_descriptor_indexing_features();
    };
}