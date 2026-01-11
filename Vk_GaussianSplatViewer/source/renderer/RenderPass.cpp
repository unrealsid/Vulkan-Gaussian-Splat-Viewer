
#include "renderer/RenderPass.h"

#include <iostream>
#include "renderer/subpasses/ForwardGeometryPass.h"
#include "renderer/subpasses/ImGuiPass.h"
#include "renderer/GPU_BufferContainer.h"
#include "renderer/subpasses/ScreenspacePass.h"
#include "renderer/subpasses/TranslucencyPass.h"
#include "structs/geometry/Vertex.h"
#include "structs/EngineContext.h"
#include "structs/scene/PushConstantBlock.h"
#include "vulkanapp/utils/RenderUtils.h"

namespace core::rendering
{
    RenderPass::RenderPass(EngineContext& engine_context, uint32_t max_frames_in_flight) :
        engine_context(engine_context), common_scene_data(nullptr),
        max_frames_in_flight(max_frames_in_flight)
    {
        swapchain_manager = engine_context.swapchain_manager.get();
        device_manager = engine_context.device_manager.get();
        buffer_container = engine_context.buffer_container.get();
        camera = engine_context.renderer->get_camera();

        depth_stencil_image = nullptr;
        command_pool = nullptr;
        camera_data = {glm::mat4{}, glm::mat4{}};
    }

    void RenderPass::allocate_and_record_command_buffers()
    {
        command_buffers.assign(max_frames_in_flight, VK_NULL_HANDLE);

        for (uint32_t i = 0; i < max_frames_in_flight; i++)
        {
            allocate_command_buffer(i);
        }
    }

    void RenderPass::renderpass_init()
    {
        create_sync_objects();
        create_renderpass_resources(true);
    }

    void RenderPass::record_subpasses(uint32_t image_index)
    {
        //TODO: Need better sync -- works for now, though
        engine_context.dispatch_table.deviceWaitIdle();

        for (auto & subpasse : subpasses)
        {
            subpasse->frame_pre_recording();
        }

        //Map data for push constants/descriptors that need it
        map_cpu_data();

        auto command_buffer = get_command_buffer(current_frame);

        engine_context.dispatch_table.resetCommandBuffer(*command_buffer, 0);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (engine_context.dispatch_table.beginCommandBuffer(*command_buffer, &begin_info) != VK_SUCCESS)
        {
            std::cout << "failed to begin recording command buffer\n";
            return;
        }

        PushConstantBlock push_constant_block{};

        for (const auto & subpass : subpasses)
        {
            subpass->init_pass_new_frame(*command_buffer, depth_stencil_image.get(), current_frame);
            subpass->record_commands(command_buffer, image_index, push_constant_block, subpass_shader_objects, *(engine_context.buffer_container));
        }

        if (engine_context.dispatch_table.endCommandBuffer(*command_buffer) != VK_SUCCESS)
        {
            std::cout << "failed to record command buffer\n";
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
            recreate_render_resources();

            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            //std::cout << "failed to acquire swapchain image. Error " << result << "\n";
            return;
        }

        record_subpasses(image_index);
        draw_frame(image_index);
    }

    void RenderPass::recreate_render_resources()
    {
        engine_context.dispatch_table.deviceWaitIdle();

        reset_subpass_command_buffers();
        recreate_swapchain();
        create_renderpass_resources(false);
        set_new_camera_aspect_ratio();
    }

    void RenderPass::cleanup()
    {
        auto dispatch_table = engine_context.dispatch_table;
        auto device = device_manager->get_device();

        dispatch_table.deviceWaitIdle();

        for (size_t i = 0; i < max_frames_in_flight; i++)
        {
            if (available_semaphores[i] != VK_NULL_HANDLE)
                dispatch_table.destroySemaphore(available_semaphores[i], nullptr);
            if (finished_semaphores[i] != VK_NULL_HANDLE)
                dispatch_table.destroySemaphore(finished_semaphores[i], nullptr);
            if (in_flight_fences[i] != VK_NULL_HANDLE)
                dispatch_table.destroyFence(in_flight_fences[i], nullptr);
        }

        if (command_pool != VK_NULL_HANDLE)
        {
            dispatch_table.destroyCommandPool(command_pool, nullptr);
            command_pool = VK_NULL_HANDLE;
        }

        if (depth_stencil_image)
        {
            if (depth_stencil_image->view != VK_NULL_HANDLE)
                dispatch_table.destroyImageView(depth_stencil_image->view, nullptr);

            if (depth_stencil_image->image != VK_NULL_HANDLE)
                vmaDestroyImage(device_manager->get_allocator(), depth_stencil_image->image, depth_stencil_image->allocation);

            depth_stencil_image.reset();
        }

        for (auto& subpass : subpasses)
        {
            subpass->cleanup();
        }
    }

    void RenderPass::init_subpasses()
    {
        //Order matters. First render an opaque geometry pass, then translucency, then the screenspace pass that combines results from the first two passes and lastly the IMGui pass for UI
        subpasses.emplace_back(std::make_unique<ForwardGeometryPass>(engine_context, max_frames_in_flight));
        subpasses.emplace_back(std::make_unique<TranslucencyPass>(engine_context, max_frames_in_flight));
        subpasses.emplace_back(std::make_unique<ScreenspacePass>(engine_context, max_frames_in_flight));
        subpasses.emplace_back(std::make_unique<ImGuiPass>(engine_context, max_frames_in_flight));

        for (const auto& subpass : subpasses)
        {
            subpass->subpass_init(subpass_shader_objects, *(engine_context.buffer_container));
        }
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
            if (depth_stencil_image->view != VK_NULL_HANDLE)
            {
                engine_context.dispatch_table.destroyImageView(depth_stencil_image->view, nullptr);
            }

            if (depth_stencil_image->image != VK_NULL_HANDLE)
            {
                vmaDestroyImage(device_manager->get_allocator(), depth_stencil_image->image, depth_stencil_image->allocation);
            }

            depth_stencil_image.reset();
        }
    }

    VkCommandBuffer* RenderPass::get_command_buffer(uint32_t image_id)
    {
        if (image_id < command_buffers.size())
        {
            return &command_buffers[image_id];
        }

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
        // for (auto& subpass : subpasses)
        // {
        //     subpass->cleanup();
        // }
        // subpasses.clear();
    }

    void RenderPass::recreate_swapchain()
    {
        auto window_manager = engine_context.window_manager.get();

        auto window_width = window_manager->get_window_width();
        auto window_height = window_manager->get_window_height();
        swapchain_manager->recreate_swapchain(window_width, window_height);
    }

    void RenderPass::create_renderpass_resources(bool is_init)
    {
        create_command_pool();

        if (is_init)
        {
            init_subpasses();
        }

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
            recreate_render_resources();

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
       available_semaphores.assign(max_frames_in_flight, VK_NULL_HANDLE);
       finished_semaphores.assign(max_frames_in_flight, VK_NULL_HANDLE);
       in_flight_fences.assign(max_frames_in_flight, VK_NULL_HANDLE);
       image_in_flight.assign(swapchain_manager->get_image_count(), VK_NULL_HANDLE);

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

    void RenderPass::set_new_camera_aspect_ratio() const
    {
        engine_context.renderer->get_camera()->set_aspect_ratio(static_cast<float>(
            swapchain_manager->get_extent().width) / static_cast<float>(swapchain_manager->get_extent().height));
    }

    void RenderPass::map_camera_data()
    {
        camera_data.projection =  camera->get_projection_matrix();
        camera_data.view = camera->get_view_matrix();
        camera_data.camera_position = glm::vec4(camera->get_position(), 1.0);

        memcpy(buffer_container->camera_data_buffer.allocation_info.pMappedData, &camera_data, sizeof(CameraData));
    }

    void RenderPass::map_cpu_data()
    {
        map_camera_data();
    }
}
