#include "vulkanapp/utils/RenderUtils.h"
#include <iostream>
#include "VkBootstrap.h"
#include "structs/engine/RenderContext.h"
#include "vulkanapp/DeviceManager.h"
#include "structs/vulkan/Vk_Image.h"

bool utils::RenderUtils::create_command_pool(const RenderContext& engine_context, VkCommandPool& out_command_pool)
{
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = engine_context.device_manager->get_device().get_queue_index(vkb::QueueType::graphics).value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    if (engine_context.dispatch_table.createCommandPool(&pool_info, nullptr, &out_command_pool) != VK_SUCCESS)
    {
        std::cout << "failed to create command pool\n";
        return false;
    }

    return true;
}

bool utils::RenderUtils::allocate_command_buffers(const RenderContext& render_context,
                                                  VkCommandPool command_pool, std::vector<VkCommandBuffer>& command_buffers)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

    if (render_context.dispatch_table.allocateCommandBuffers(&allocInfo, command_buffers.data()) != VK_SUCCESS)
    {
        return false;
    }
    return true;
}

bool utils::RenderUtils::allocate_command_buffer(const RenderContext& render_context, VkCommandPool command_pool, VkCommandBuffer& command_buffer)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (render_context.dispatch_table.allocateCommandBuffers(&allocInfo, &command_buffer) != VK_SUCCESS)
    {
        return false;
    }
    return true;
}

VkBool32 utils::RenderUtils::get_supported_depth_stencil_format(VkPhysicalDevice physical_device, VkFormat* depth_stencil_format)
{
    std::vector formatList =
    {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
    };

    for (auto& format : formatList)
    {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &formatProps);
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            *depth_stencil_format = format;
            return true;
        }
    }

    return false;
}

bool utils::RenderUtils::create_depth_stencil_image(const RenderContext& engine_context, VkExtent2D extents,
    VmaAllocator allocator, Vk_Image& depth_image)
{
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.imageType = VK_IMAGE_TYPE_2D;
    imageCI.format = depth_image.format;
    imageCI.extent = { extents.width, extents.height, 1 };
    imageCI.mipLevels = 1;
    imageCI.arrayLayers = 1;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(allocator, &imageCI, &allocInfo, &depth_image.image, &depth_image.allocation, nullptr) != VK_SUCCESS)
    {
        std::cerr << "failed to create depth stencil image!";
        return true;
    }

    VkImageViewCreateInfo imageViewCI{};
    imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCI.image = depth_image.image;
    imageViewCI.format = depth_image.format;
    imageViewCI.subresourceRange.baseMipLevel = 0;
    imageViewCI.subresourceRange.levelCount = 1;
    imageViewCI.subresourceRange.baseArrayLayer = 0;
    imageViewCI.subresourceRange.layerCount = 1;
    imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    
    if (depth_image.format >= VK_FORMAT_D16_UNORM_S8_UINT)
    {
        imageViewCI.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    if (engine_context.dispatch_table.createImageView(&imageViewCI, nullptr,  &depth_image.view) != VK_SUCCESS)
    {
        std::cerr << "failed to create depth stencil image view!";
        return false;
    }

    return true;
}

VkRenderingInfoKHR utils::RenderUtils::rendering_info(VkRect2D render_area, uint32_t color_attachment_count,
    const VkRenderingAttachmentInfoKHR* pColorAttachments, VkRenderingFlagsKHR flags)
{
    VkRenderingInfoKHR rendering_info   = {};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.pNext                = VK_NULL_HANDLE;
    rendering_info.flags                = flags;
    rendering_info.renderArea           = render_area;
    rendering_info.layerCount           = 1;
    rendering_info.viewMask             = 0;
    rendering_info.colorAttachmentCount = color_attachment_count;
    rendering_info.pColorAttachments    = pColorAttachments;
    rendering_info.pDepthAttachment     = VK_NULL_HANDLE;
    rendering_info.pStencilAttachment   = VK_NULL_HANDLE;
    return rendering_info;
}