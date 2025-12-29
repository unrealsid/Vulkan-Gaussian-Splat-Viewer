
#include "config/Config.inl"
#include "renderer/Renderer.h"
#include "structs/EngineContext.h"
#include "../../include/structs/geometry/Vertex2D.h"
#include "vulkanapp/VulkanCleanupQueue.h"
#include "vulkanapp/utils/MemoryUtils.h"

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
}
