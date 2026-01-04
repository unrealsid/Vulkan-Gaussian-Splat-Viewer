

#include "renderer/Renderer.h"
#include "config/Config.inl"
#include "structs/EngineContext.h"
#include "structs/geometry/Vertex2D.h"
#include "vulkanapp/VulkanCleanupQueue.h"
#include "vulkanapp/utils/MemoryUtils.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "structs/scene/CameraData.h"

namespace core::renderer
{
    void Renderer::init_vulkan()
    {
        create_device();
        create_swapchain();
        utils::MemoryUtils::create_vma_allocator(*engine_context.device_manager);
    }

    void Renderer::create_swapchain() const
    {
        engine_context.swapchain_manager = std::make_unique<vulkanapp::SwapchainManager>(engine_context);
        engine_context.swapchain_manager->initialize(engine_context.device_manager->get_physical_device(),
                                                      engine_context.device_manager->get_device(),
                                                      //Windowing
                                                      engine_context.device_manager->get_surface(),
                                                      2,
                                                      engine_context.window_manager->get_window_width(),
                                                      engine_context.window_manager->get_window_height());
    }

    void Renderer::create_device() const
    {
        engine_context.device_manager = std::make_unique<vulkanapp::DeviceManager>(engine_context);
        engine_context.device_manager->device_init();
        engine_context.device_manager->init_queues();
    }

    void Renderer::init_cleanup() const
    {
        // Pushing in the order we want them to be destroyed in REVERSE
        // LIFO means the last one pushed is the first one executed.

        // 5. Device manager should be destroyed last (contains VMA allocator and VkDevice)
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context.device_manager->cleanup()));

        // 4. Swapchain
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context.swapchain_manager->cleanup()));

        // 2. Render pass cleanup (includes destroying sync objects, command pool, depth image)
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(render_pass->cleanup()));

        // 1. Wait for idle must be the VERY FIRST thing we do when flushing the queue.
        // It's already called inside render_pass->cleanup(), but we can add it explicitly as the last push (first to execute)
        // just to be sure.
        vulkanapp::VulkanCleanupQueue::push_cleanup_function([this]() { engine_context.dispatch_table.deviceWaitIdle(); });
    }

    void Renderer::cleanup()
    {
        vulkanapp::VulkanCleanupQueue::flush();
    }

    void Renderer::create_camera_buffer(uint32_t width, uint32_t height)
    {
        CameraData ubo{};

        auto swapchain_manager = engine_context.swapchain_manager.get();
        first_person_camera = std::make_unique<camera::FirstPersonCamera>(glm::vec3(0.0f, 0.0f, 3.0f), 45.0f,
            static_cast<float>(swapchain_manager->get_extent().width ) / static_cast<float>(swapchain_manager->get_extent().height));

        ubo.projection = first_person_camera->get_projection_matrix();
        ubo.view = first_person_camera->get_view_matrix();

        utils::MemoryUtils::allocate_buffer_with_mapped_access(
                engine_context.dispatch_table,
                engine_context.device_manager->get_allocator(),
                sizeof(CameraData),
                common_scene_data->camera_data_buffer
            );

        memcpy(common_scene_data->camera_data_buffer.allocation_info.pMappedData, &ubo, sizeof(CameraData));

        vmaFlushAllocation(
            engine_context.device_manager->get_allocator(),
            common_scene_data->camera_data_buffer .allocation,
            0,
            VK_WHOLE_SIZE
        );
    }


}
