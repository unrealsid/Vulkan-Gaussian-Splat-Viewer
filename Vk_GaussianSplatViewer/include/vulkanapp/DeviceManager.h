#pragma once
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <VkBootstrap.h>

struct EngineContext;

//Manages instance, device and queues
namespace vulkanapp
{
    class DeviceManager
    {
    public:
        DeviceManager(EngineContext& engine_context);
        ~DeviceManager();
        
        bool device_init();
        bool get_queues();
        void cleanup();
        
    private:
        vkb::Instance instance;
        VkSurfaceKHR surface;
        vkb::Device device;
        vkb::PhysicalDevice physical_device;

        VkQueue compute_queue;
        VkQueue graphics_queue;
        VkQueue present_queue;

        VmaAllocator vmaAllocator;

        EngineContext& engine_context;
        
    public:
        [[nodiscard]] vkb::Instance get_instance() const { return instance; }
        [[nodiscard]] VkSurfaceKHR get_surface() const { return surface; }
        [[nodiscard]] vkb::Device get_device() const { return device; }
        [[nodiscard]] vkb::PhysicalDevice get_physical_device() const { return physical_device; }
        [[nodiscard]] VkQueue get_graphics_queue() const { return graphics_queue; }
        [[nodiscard]] VkQueue get_present_queue() const { return present_queue; }
        [[nodiscard]] VkQueue get_compute_queue() const { return compute_queue; }
        [[nodiscard]] VmaAllocator get_allocator() const { return vmaAllocator; }

        void set_vma_allocator(VmaAllocator allocator) { vmaAllocator = allocator; }
    };
}

