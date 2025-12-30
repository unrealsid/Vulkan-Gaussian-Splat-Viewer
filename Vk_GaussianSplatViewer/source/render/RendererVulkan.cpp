

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

        // 3. Buffers should be destroyed before the device/allocator but after waiting for idle
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), engine_context.mesh_vertices_buffer)));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), engine_context.mesh_indices_buffer)));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), engine_context.gaussian_buffer)));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), engine_context.camera_data_buffer)));

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

    template<typename V>
    void Renderer::allocate_mesh_buffers(const std::vector<V>& vertices, const std::vector<uint32_t>& indices)
    {
        //Make Vulkan resources for mesh
        utils::MemoryUtils::create_vertex_and_index_buffers<Vertex2D>(engine_context,
                vertices,
                indices,
                engine_context.renderer->get_render_pass()->get_command_pool(),
                engine_context.mesh_vertices_buffer, engine_context.mesh_indices_buffer);
    }

    template void Renderer::allocate_mesh_buffers(const std::vector<Vertex2D>& vertices, const std::vector<uint32_t>& indices);

    void Renderer::allocate_gaussian_buffer(const std::vector<GaussianSurface>& gaussians) const
    {
        utils::MemoryUtils::create_vertex_buffer_with_staging(engine_context,
            gaussians,
            engine_context.renderer->get_render_pass()->get_command_pool(),
            engine_context.gaussian_buffer);
    }


    void Renderer::create_camera_buffer(uint32_t width, uint32_t height)
    {
        CameraData ubo;

        glm::vec3 cameraPos = glm::vec3(0.0, 10.0, 0.0);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);

        ubo.view = glm::lookAt(cameraPos, target, up);

        float fov = glm::radians(45.0f);
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        ubo.projection = glm::perspective(fov, aspect, nearPlane, farPlane);

        // Flip Y for Vulkan
        ubo.projection[1][1] *= -1.0f;

        utils::MemoryUtils::allocate_buffer_with_mapped_access(
                engine_context.dispatch_table,
                engine_context.device_manager->get_allocator(),
                sizeof(CameraData),
                engine_context.camera_data_buffer
            );

        memcpy(engine_context.camera_data_buffer.allocation_info.pMappedData, &ubo, sizeof(CameraData));

        vmaFlushAllocation(
            engine_context.device_manager->get_allocator(),
            engine_context.camera_data_buffer .allocation,
            0,
            VK_WHOLE_SIZE
        );
    }


}
