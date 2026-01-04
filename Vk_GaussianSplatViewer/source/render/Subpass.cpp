#include "renderer/Subpass.h"

#include "structs/EngineContext.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/RenderUtils.h"

namespace core::renderer
{
    Subpass::Subpass(EngineContext& engine_context, uint32_t max_frames_in_flight) :
                    engine_context(engine_context),
                    max_frames_in_flight(max_frames_in_flight),
                    active_command_buffer(nullptr), depth_stencil_image(nullptr)
    {
        swapchain_manager = engine_context.swapchain_manager.get();
        device_manager = engine_context.device_manager.get();
    }

    void Subpass::init_pass_new_frame(VkCommandBuffer p_command_buffer, Vk_Image* p_depth_stencil_image, uint32_t p_frame)
    {
        current_frame = p_frame;
        active_command_buffer = p_command_buffer;
        depth_stencil_image = p_depth_stencil_image;
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

    void Subpass::set_present_image_transition(uint32_t image_id, PresentationImageType presentation_image_type) const
    {

        switch (presentation_image_type)
        {

        case PresentationImageType::SwapChain:
            {
            auto swapchain_ref = engine_context.swapchain_manager.get();
            auto image = swapchain_ref->get_images()[image_id];

            utils::ImageUtils::image_layout_transition(active_command_buffer,
                                            image.image,
                                            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                            0,
                                            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                            VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
            break;
            }

        case PresentationImageType::DepthStencil:
            utils::ImageUtils::image_layout_transition(active_command_buffer,
                                          depth_stencil_image->image,
                                         VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                                         VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                                         0,
                                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                          VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 });

            break;

        default:
            std::cout << "Invalid PresentationImageType\n";
        }
    }

    void Subpass::setup_color_attachment(uint32_t image, VkClearValue clear_value)
    {

        auto swapchain_ref = engine_context.swapchain_manager.get();
        color_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };

        //TODO: Bounds check

        color_attachment_info.pNext        = VK_NULL_HANDLE;
        color_attachment_info.imageView    = swapchain_ref->get_images()[image].image_view;
        color_attachment_info.imageLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.resolveMode  = VK_RESOLVE_MODE_NONE;
        color_attachment_info.loadOp       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp      = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue   = clear_value;
    }

    void Subpass::setup_depth_attachment(VkClearValue clear_value)
    {

        // Depth attachment
        depth_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depth_attachment_info.pNext       = VK_NULL_HANDLE;
        depth_attachment_info.imageView   = depth_stencil_image->view;
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

    void Subpass::end_command_buffer_recording(uint32_t image, bool last_subpass)
    {
        if (last_subpass)
        {
            utils::ImageUtils::image_layout_transition
            (
                 active_command_buffer,                            // Command buffer
                 engine_context.swapchain_manager->get_images()[image].image,    // Swapchain image
                 VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, // Source pipeline stage
                 VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,     // Destination pipeline stage
                 VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,     // Source access mask
                 0,                                        // Destination access mask
                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // Old layout
                 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // New layout
                  VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
        }
    }

    void Subpass::set_material(const std::shared_ptr<material::Material>& material)
    {
        material_to_use = material;
    }

    void Subpass::cleanup()
    {
        if (material_to_use)
        {
            material_to_use->cleanup();
        }
    }
}
