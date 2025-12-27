#pragma once
#include <vector>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include "structs/Vk_Image.h"

struct RenderContext;
struct DepthStencilImage;

namespace utils
{
    class RenderUtils
    {
    public:
        static bool create_command_pool(const RenderContext& engine_context, VkCommandPool& out_command_pool);

        static bool allocate_command_buffers(const RenderContext& render_context, VkCommandPool command_pool, std::vector<VkCommandBuffer>& command_buffers);

        static bool allocate_command_buffer(const RenderContext& render_context, VkCommandPool command_pool, VkCommandBuffer& command_buffer);

        static VkBool32 get_supported_depth_stencil_format(VkPhysicalDevice physical_device, VkFormat* depth_stencil_format);

        static bool create_depth_stencil_image(const RenderContext& engine_context, VkExtent2D extents, VmaAllocator allocator, Vk_Image& depth_image);

        static VkRenderingInfoKHR rendering_info(VkRect2D render_area = {},
                                      uint32_t color_attachment_count = 0,
                                      const VkRenderingAttachmentInfoKHR *pColorAttachments = VK_NULL_HANDLE,
                                      VkRenderingFlagsKHR flags = 0);
    };
}
