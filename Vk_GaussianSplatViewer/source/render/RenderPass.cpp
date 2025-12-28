
#include "renderer/RenderPass.h"

#include <iostream>
#include "renderer/subpasses/GeometryPass.h"
#include "structs/PushConstantBlock.h"
#include "structs/Vertex.h"
#include "structs/EngineContext.h"
#include "vulkanapp/utils/RenderUtils.h"


namespace core::renderer
{
    RenderPass::RenderPass(EngineContext& engine_context, uint32_t max_frames_in_flight) :
        engine_context(engine_context),
        max_frames_in_flight(max_frames_in_flight)
    {
        swapchain_manager = engine_context.swapchain_manager.get();
        device_manager = engine_context.device_manager.get();
        depth_stencil_image = nullptr;
        command_buffers.resize(max_frames_in_flight);
    }

    void RenderPass::allocate_and_record_command_buffers()
    {
        command_buffers.resize(max_frames_in_flight);

        for (uint32_t i = 0; i < max_frames_in_flight; i++)
        {
            allocate_command_buffer(i);
        }

        //TODO: Need to remove this later. Causes null command buffer validation errors
        /*for (uint32_t i = 0; i < max_frames_in_flight; i++)
        {
            record_subpasses(i);
        }*/
    }

    void RenderPass::init_renderpass()
    {
        create_sync_objects();
        create_rendering_resources();
    }

    void RenderPass::record_subpasses(uint32_t image_index)
    {
        //TODO: Need better sync -- works for now, though
        engine_context.dispatch_table.deviceWaitIdle();

        auto command_buffer = get_command_buffer(current_frame);
        for (const auto& subpass : subpasses)
        {
            subpass->init_pass_new_frame(*command_buffer, depth_stencil_image.get(), current_frame);
            subpass->record_commands(command_buffer, image_index);
        }
    }

    void RenderPass::record_commands_and_draw()
    {
        uint32_t image_index = 0;
        auto dispatch_table = engine_context.dispatch_table;

        // We need to acquire the image before recording because we need image_index for layout transitions
        VkResult result = dispatch_table.acquireNextImageKHR(swapchain_manager->get_swapchain(), UINT64_MAX, available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            dispatch_table.deviceWaitIdle();

            reset_subpass_command_buffers();
            recreate_swapchain();
            create_rendering_resources();

            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            std::cout << "failed to acquire swapchain image. Error " << result << "\n";
            return;
        }

        record_subpasses(image_index);
        draw_frame(image_index);
    }

    void RenderPass::init_subpasses()
    {
        subpasses.emplace_back(std::make_unique<GeometryPass>(engine_context, max_frames_in_flight));
    }

    void RenderPass::create_command_pool()
    {
        utils::RenderUtils::create_command_pool(engine_context, command_pool);
    }

    void RenderPass::reset_command_pool()
    {
        if (command_pool != VK_NULL_HANDLE)
        {
            engine_context.dispatch_table.destroyCommandPool(command_pool, 0);
            command_buffers.clear();
            command_pool = VK_NULL_HANDLE;
        }

        if (depth_stencil_image)
        {
            engine_context.dispatch_table.destroyImage(depth_stencil_image->image, nullptr);
            depth_stencil_image = {};
        }
    }

    VkCommandBuffer* RenderPass::get_command_buffer(uint32_t image_id)
    {
        if (image_id < command_buffers.size())
            return &command_buffers[image_id];

        return nullptr;
    }

    void RenderPass::allocate_command_buffer(uint32_t image)
    {
        if (VkCommandBuffer* command_buffer = get_command_buffer(image))
        {
            utils::RenderUtils::allocate_command_buffer(engine_context, command_pool, *command_buffer);
        }
    }

    void RenderPass::create_depth_stencil_image()
    {
        depth_stencil_image = std::make_unique<Vk_Image>();
        utils::RenderUtils::get_supported_depth_stencil_format(device_manager->get_physical_device(), &depth_stencil_image->format);
        utils::RenderUtils::create_depth_stencil_image(engine_context,
                                                       swapchain_manager->get_extent(),
                                                        device_manager->get_allocator(),
                                                       *depth_stencil_image);
    }

    void RenderPass::reset_subpass_command_buffers()
    {
        reset_command_pool();
        subpasses.clear();
    }

    void RenderPass::recreate_swapchain()
    {
        auto window_manager = engine_context.window_manager.get();

        auto window_width = window_manager->get_window_width();
        auto window_height = window_manager->get_window_height();
        swapchain_manager->recreate_swapchain(window_width, window_height);
    }

    void RenderPass::create_rendering_resources()
    {
        init_subpasses();
        create_command_pool();
        create_depth_stencil_image();

        allocate_and_record_command_buffers();
    }

    bool RenderPass::draw_frame(uint32_t image_index)
    {
        auto dispatch_table = engine_context.dispatch_table;
        dispatch_table.waitForFences(1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

        if (image_in_flight[image_index] != VK_NULL_HANDLE)
        {
            dispatch_table.waitForFences(1, &image_in_flight[image_index], VK_TRUE, UINT64_MAX);
        }

        image_in_flight[image_index] = in_flight_fences[current_frame];

        VkSubmitInfo2 submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

        VkSemaphoreSubmitInfo waitSemaphoreSubmitInfo = {};
        waitSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        waitSemaphoreSubmitInfo.semaphore = available_semaphores[current_frame];
        waitSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = &waitSemaphoreSubmitInfo;

        VkCommandBufferSubmitInfo commandBufferSubmitInfo = {};
        commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferSubmitInfo.commandBuffer = command_buffers[current_frame];

        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

        VkSemaphoreSubmitInfo signalSemaphoreSubmitInfo = {};
        signalSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signalSemaphoreSubmitInfo.semaphore = finished_semaphores[current_frame];
        signalSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; // Signal when all commands are done

        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &signalSemaphoreSubmitInfo;

        dispatch_table.resetFences(1, &in_flight_fences[current_frame]);
        if (dispatch_table.queueSubmit2(device_manager->get_graphics_queue(), 1, &submitInfo, in_flight_fences[current_frame]) != VK_SUCCESS)
        {
            std::cout << "failed to submit draw command buffer\n";
            return false;
        }

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        VkSemaphore signal_semaphores[] = { finished_semaphores[current_frame] };
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapChains[] = { swapchain_manager->get_swapchain() };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapChains;

        present_info.pImageIndices = &image_index;

        VkResult result = dispatch_table.queuePresentKHR(device_manager->get_present_queue(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            dispatch_table.deviceWaitIdle();

            reset_subpass_command_buffers();
            recreate_swapchain();
            create_rendering_resources();

            return true;
        }

        if (result != VK_SUCCESS)
        {
            std::cout << "failed to present swapchain image\n";
            return false;
        }

        current_frame = (current_frame + 1) % max_frames_in_flight;
        return true;
    }

    bool RenderPass::create_sync_objects()
   {
       available_semaphores.resize(max_frames_in_flight);
       finished_semaphores.resize(max_frames_in_flight);
       in_flight_fences.resize(max_frames_in_flight);
       image_in_flight.resize(swapchain_manager->get_image_count(), VK_NULL_HANDLE);

       VkSemaphoreCreateInfo semaphore_info = {};
       semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

       VkFenceCreateInfo fence_info = {};
       fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
       fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

       auto dispatch_table = engine_context.dispatch_table;

       for (size_t i = 0; i < max_frames_in_flight; i++)
       {
           if (dispatch_table.createSemaphore(&semaphore_info, nullptr, &available_semaphores[i]) != VK_SUCCESS ||
               dispatch_table.createSemaphore(&semaphore_info, nullptr, &finished_semaphores[i]) != VK_SUCCESS ||
               dispatch_table.createFence(&fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS)
           {
               std::cout << "failed to create sync objects\n";
               return false;
           }
       }

       return true;
   }
}
