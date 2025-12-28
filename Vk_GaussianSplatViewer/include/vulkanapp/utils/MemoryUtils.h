#pragma once
#include <unordered_map>
#include <vector>

#include "VkBootstrapDispatch.h"
#include "structs/Vk_Image.h"
#include "structs/Vertex.h"
#include "vulkanapp/DeviceManager.h"

struct GPU_Buffer;
struct EngineContext;
struct MaterialParams;
struct VMAAllocator;

namespace vulkan
{
    class DeviceManager;
}

namespace utils
{
    class MemoryUtils
    {
    public: 
        static void create_vma_allocator(vulkanapp::DeviceManager& device_manager);

        static void create_buffer(vkb::DispatchTable dispatch_table, VmaAllocator allocator, VkDeviceSize size, VkBufferUsageFlags usage,
                                    VmaMemoryUsage memory_usage, VmaAllocationCreateFlags vmaAllocationFlags, GPU_Buffer& out_buffer);

        static VkResult map_persistent_data(VmaAllocator vmaAllocator, VmaAllocation allocation, const VmaAllocationInfo& allocationInfo, const void* data, VkDeviceSize bufferSize, size_t offset_in_buffer = 0);

        static VkDeviceAddress get_buffer_device_address(const vkb::DispatchTable& disp, VkBuffer buffer);

        static void copy_buffer(vkb::DispatchTable disp, VkQueue queue, VkCommandPool command_pool, VkBuffer srcBuffer, VkBuffer dst_buffer, VkDeviceSize size);

        template<typename V>
        static void create_vertex_and_index_buffers(EngineContext& render_context,
                                                   const std::vector<V>& vertices,
                                                   const std::vector<uint32_t>& indices,
                                                   VkCommandPool command_pool,
                                                   GPU_Buffer& out_vertex_buffer, GPU_Buffer& out_index_buffer);
        
        //Creates a device-addressable buffer (Can be addressed via vulkan BDA)
        static void allocate_buffer_with_mapped_access(const vkb::DispatchTable& dispatch_table, VmaAllocator allocator, VkDeviceSize size, GPU_Buffer& buffer);

        //Creates a buffer that is persistently mapped
        static void allocate_buffer_with_random_access(const vkb::DispatchTable& dispatch_table, VmaAllocator allocator, VkDeviceSize size, GPU_Buffer& buffer);

    };
}
