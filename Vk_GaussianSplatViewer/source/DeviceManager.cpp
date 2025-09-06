#include "DeviceManager.h"

#include <iostream>
#include <vulkan/vulkan.h>

vulkan_app::DeviceManager::DeviceManager(): surface(nullptr), graphics_queue(nullptr),
                                        present_queue(nullptr),
                                        vmaAllocator(nullptr)
{
}

vulkan_app::DeviceManager::~DeviceManager()
{
    vkDestroyDevice(device, nullptr);
}

bool vulkan_app::DeviceManager::device_init()
{
    // Create the disable feature struct
    VkValidationFeatureDisableEXT disables[] =
    {
        VK_VALIDATION_FEATURE_DISABLE_UNIQUE_HANDLES_EXT
    };
    
    vkb::InstanceBuilder instance_builder;
    auto instance_ret = instance_builder.
        set_minimum_instance_version(VK_API_VERSION_1_4)
        .use_default_debug_messenger()
        .add_validation_feature_disable(*disables)
        //.enable_layer("VK_LAYER_KHRONOS_shader_object")
        .enable_extension(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME)
        .enable_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)
        .enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
        .request_validation_layers()
        .build();
    
    if (!instance_ret)
    {
        std::cout << instance_ret.error().message() << "\n";
        return false;
    }
    instance = instance_ret.value();

    auto instance_dispatch_table = instance.make_table();

    VkPhysicalDeviceFeatures features = {};
    features.geometryShader = VK_FALSE;
    features.tessellationShader = VK_FALSE;

    vkb::PhysicalDeviceSelector phys_device_selector(instance);
    auto phys_device_ret = phys_device_selector
        .add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
        .add_required_extension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)
        .add_required_extension(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)
        .add_required_extension(VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)
        .add_required_extension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME)
        .add_required_extension(VK_EXT_SHADER_OBJECT_EXTENSION_NAME)
        .add_required_extension(VK_KHR_MULTIVIEW_EXTENSION_NAME)
        .add_required_extension(VK_KHR_MAINTENANCE_2_EXTENSION_NAME)
        .add_required_extension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)
        .add_required_extension(VK_KHR_DEVICE_GROUP_EXTENSION_NAME)
        .add_required_extension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME) 
        .add_required_extension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME)
        .add_required_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME)
        .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
        .add_required_extension(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME)
        .add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
        //.add_required_extension(VK_KHR_MAINTENANCE_6_EXTENSION_NAME)
        .set_required_features(features)
        .set_surface(surface)
        .select();

    return true;
}

bool vulkan_app::DeviceManager::get_queues()
{
    auto gq = device.get_queue(vkb::QueueType::graphics);
    if (!gq.has_value())
    {
        std::cout << "failed to get graphics queue: " << gq.error().message() << "\n";
        return false;
    }
    graphics_queue = gq.value();

    auto pq = device.get_queue(vkb::QueueType::present);
    if (!pq.has_value())
    {
        std::cout << "failed to get present queue: " << pq.error().message() << "\n";
        return false;
    }
    present_queue = pq.value();
    return true;
}
