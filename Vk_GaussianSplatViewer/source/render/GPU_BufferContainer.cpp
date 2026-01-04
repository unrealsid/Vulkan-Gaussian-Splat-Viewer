#include "renderer/GPU_BufferContainer.h"

#include "structs/scene/CameraData.h"
#include "vulkanapp/utils/MemoryUtils.h"

namespace core::renderer
{
    GPU_BufferContainer::GPU_BufferContainer(EngineContext& engine_context) : engine_context(engine_context)
    {
    }

    void GPU_BufferContainer::allocate_camera_buffer(const camera::FirstPersonCamera& first_person_camera)
    {
        CameraData ubo{};

        ubo.projection = first_person_camera.get_projection_matrix();
        ubo.view = first_person_camera.get_view_matrix();

        utils::MemoryUtils::allocate_buffer_with_mapped_access(
            engine_context.dispatch_table,
            engine_context.device_manager->get_allocator(),
            sizeof(CameraData),
            camera_data_buffer
        );

        memcpy(camera_data_buffer.allocation_info.pMappedData, &ubo, sizeof(CameraData));

        vmaFlushAllocation(
            engine_context.device_manager->get_allocator(),
            camera_data_buffer .allocation,
            0,
            VK_WHOLE_SIZE
        );
    }

    void GPU_BufferContainer::allocate_gaussian_surface_buffer(const std::vector<GaussianSurface>& gaussians)
    {
        engine_context.dispatch_table.deviceWaitIdle();

        auto device_manager = engine_context.device_manager.get();

        vmaDestroyBuffer(device_manager->get_allocator(), gaussian_buffer.buffer, gaussian_buffer.allocation);
        gaussian_buffer = { VK_NULL_HANDLE, VK_NULL_HANDLE, {}, {} };

        utils::MemoryUtils::create_vertex_buffer_with_staging(engine_context,
                                                              gaussians,
                                                              engine_context.renderer->get_render_pass()->get_command_pool(),
                                                              gaussian_buffer);
    }
}