#pragma once
#include <iostream>

#include "VkBootstrapDispatch.h"
#include "structs/LoadedImageData.h"
#include "structs/Vk_Image.h"

struct RenderContext;

namespace vulkan
{
    class DeviceManager;
}

struct GPU_Buffer;
struct Init;

namespace utils
{
    class ImageUtils
    {
    public:    
        static LoadedImageData load_image_data(const std::string& file_path, int desired_channels = 4);

        static Vk_Image create_texture_image(RenderContext& engine_context, VkCommandPool command_pool, const LoadedImageData& image_data);

        static VkImageCreateInfo image_create_info(VkFormat image_format, VkImageUsageFlags image_usage_flags, VkExtent3D image_extent);

        static void copy_image(RenderContext& engine_context, VkQueue queue, VkCommandPool command_pool, GPU_Buffer srcBuffer, Vk_Image dstImage, VkDeviceSize size, VkExtent3D extend);

        static void copy_image_to_buffer(RenderContext& render_context, Vk_Image src_image, GPU_Buffer& dst_buffer, VkCommandBuffer cmd_buffer, VkOffset3D image_offset);

        static void create_image_sampler(const vkb::DispatchTable& disp, Vk_Image& image, VkFilter filter);

        static void create_image_view(const vkb::DispatchTable& disp, Vk_Image& image, VkFormat format);

        static void image_layout_transition(VkCommandBuffer command_buffer,
                                    VkImage image,
                                    VkPipelineStageFlags src_stage_mask,
                                    VkPipelineStageFlags dst_stage_mask,
                                    VkAccessFlags src_access_mask,
                                    VkAccessFlags dst_access_mask,
                                    VkImageLayout old_layout,
                                    VkImageLayout new_layout,
                                    const VkImageSubresourceRange &subresource_range);
    };
};
