

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
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context.swapchain_manager->cleanup()));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context.device_manager->cleanup()));
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

        // Camera position: 10 units away on the Z axis
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);  // Looking at origin
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);      // Y-up

        // Create view matrix
        ubo.view = glm::lookAt(cameraPos, target, up);

        // Create projection matrix (perspective)
        float fov = glm::radians(45.0f);  // 45 degree field of view
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        ubo.projection = glm::perspective(fov, aspect, nearPlane, farPlane);

        // IMPORTANT: Vulkan uses different clip space than OpenGL
        // Vulkan's Y axis is flipped and Z range is [0, 1] instead of [-1, 1]
        ubo.projection[1][1] *= -1.0f;  // Flip Y coordinate

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
