#include "renderer/GPU_BufferContainer.h"

#include <ranges>

#include "renderer/RenderPass.h"
#include "structs/EngineContext.h"
#include "structs/scene/CameraData.h"
#include "vulkanapp/utils/MemoryUtils.h"

namespace core::rendering
{
    GPU_BufferContainer::GPU_BufferContainer(EngineContext& engine_context) : engine_context(engine_context)
    {
    }

    void GPU_BufferContainer::allocate_camera_buffer(const camera::FirstPersonCamera& first_person_camera)
    {
        CameraData ubo{};

        ubo.projection = first_person_camera.get_projection_matrix();
        ubo.view = first_person_camera.get_view_matrix();
        ubo.camera_position = glm::vec4(first_person_camera.get_position(), 1.0);

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

    void GPU_BufferContainer::set_buffer(const std::string& buffer_name, const GPU_Buffer& buffer)
    {
        gaussian_buffers[buffer_name] = buffer;
    }

    GPU_Buffer* GPU_BufferContainer::get_buffer(const std::string& buffer_name)
    {
        const auto it = gaussian_buffers.find(buffer_name);
        if (it == gaussian_buffers.end())
            return nullptr;

        return &it->second;
    }

    void GPU_BufferContainer::destroy_buffers()
    {
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), camera_data_buffer);

        for (auto& val : gaussian_buffers | std::views::values)
        {
            utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), val);
        }
    }

    template <typename N>
    void GPU_BufferContainer::allocate_gaussian_buffer(const std::string& buffer_name, const std::vector<N>& gaussian_parameters)
    {
        engine_context.dispatch_table.deviceWaitIdle();

        auto device_manager = engine_context.device_manager.get();

        if (auto buffer = get_buffer(buffer_name))
        {
            vmaDestroyBuffer(device_manager->get_allocator(), buffer->buffer, buffer->allocation);
            *buffer = { VK_NULL_HANDLE, VK_NULL_HANDLE, {}, {} };
            gaussian_buffers.erase(buffer_name);
        }

        GPU_Buffer new_buffer;

        utils::MemoryUtils::create_vertex_buffer_with_staging(engine_context,
                                                              gaussian_parameters,
                                                              engine_context.renderer->get_render_pass()->get_command_pool(),
                                                              new_buffer);

        set_buffer(buffer_name, new_buffer);
    }

    template void GPU_BufferContainer::allocate_gaussian_buffer(const std::string& buffer_name, const std::vector<glm::vec4>& gaussian_parameters);
    template void GPU_BufferContainer::allocate_gaussian_buffer(const std::string& buffer_name, const std::vector<float>& gaussian_parameters);

}