#pragma once

#include "camera/FirstPersonCamera.h"
#include "renderer/Subpass.h"
#include "structs/geometry/GaussianSurface.h"
#include "structs/scene/CameraData.h"


namespace core::renderer
{
    class GPU_BufferContainer;

    class GeometryPass : public Subpass
    {
    public:
        GeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight);

        void frame_pre_recording() override;

        void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, bool is_last) override;

        void cleanup() override;

    private:
        camera::FirstPersonCamera* camera;
        VkExtent2D extents{};
        CameraData camera_data{};
        GPU_BufferContainer* buffer_container;
    };
}
