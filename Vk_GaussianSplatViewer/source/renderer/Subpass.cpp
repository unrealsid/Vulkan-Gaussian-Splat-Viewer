#include "renderer/Subpass.h"

#include "structs/EngineContext.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/RenderUtils.h"

namespace core::rendering
{
    Subpass::Subpass(EngineContext& engine_context, uint32_t max_frames_in_flight) :
        engine_context(engine_context),
        max_frames_in_flight(max_frames_in_flight),
        active_command_buffer(nullptr), color_attachment_info({}), depth_attachment_info({})
    {
        swapchain_manager = engine_context.swapchain_manager.get();
        device_manager = engine_context.device_manager.get();
    }

    void Subpass::init_pass_new_frame(VkCommandBuffer p_command_buffer, uint32_t p_frame)
    {
        current_frame = p_frame;
        active_command_buffer = p_command_buffer;
    }

    void Subpass::begin_command_buffer_recording() const
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (engine_context.dispatch_table.beginCommandBuffer(active_command_buffer, &begin_info) != VK_SUCCESS)
        {
            std::cout << "failed to begin recording command buffer\n";
            assert(false);
        }
    }

    void Subpass::setup_color_attachment(uint32_t image, VkClearValue clear_value)
    {
        auto swapchain_ref = engine_context.swapchain_manager.get();
        color_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };

        color_attachment_info.pNext        = VK_NULL_HANDLE;
        color_attachment_info.imageView    = swapchain_ref->get_images()[image].image_view;
        color_attachment_info.imageLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.resolveMode  = VK_RESOLVE_MODE_NONE;
        color_attachment_info.loadOp       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp      = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue   = clear_value;
    }

    void Subpass::setup_depth_attachment(const Vk_Image& depth_image, VkClearValue clear_value)
    {
        // Depth attachment
        depth_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depth_attachment_info.pNext       = VK_NULL_HANDLE;
        depth_attachment_info.imageView   = depth_image.view;
        depth_attachment_info.imageLayout =  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
        depth_attachment_info.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_info.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment_info.clearValue  = { {1.0f, 0} }; // depth=1, stencil=0
    }

    void Subpass::begin_rendering()
    {
        // Render area
        VkRect2D render_area = { {0, 0}, {swapchain_manager->get_extent().width, swapchain_manager->get_extent().height} };

        // Dynamic rendering info
        VkRenderingInfoKHR render_info = { VK_STRUCTURE_TYPE_RENDERING_INFO_KHR };
        render_info.renderArea          = render_area;
        render_info.layerCount          = 1;
        render_info.colorAttachmentCount = 1;
        render_info.pColorAttachments   = &color_attachment_info;
        render_info.pDepthAttachment    = &depth_attachment_info;
        render_info.pStencilAttachment  = &depth_attachment_info;

        engine_context.dispatch_table.cmdBeginRenderingKHR(active_command_buffer, &render_info);
    }

    void Subpass::end_rendering()
    {
        engine_context.dispatch_table.cmdEndRenderingKHR(active_command_buffer);
    }

    void Subpass::cleanup()
    {

    }
}
