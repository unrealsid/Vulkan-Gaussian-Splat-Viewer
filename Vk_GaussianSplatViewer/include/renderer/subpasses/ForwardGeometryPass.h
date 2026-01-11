#pragma once

#include "camera/FirstPersonCamera.h"
#include "renderer/Subpass.h"
#include "structs/geometry/GaussianSurface.h"

/*
 * Used to draw opaque geometry
 */

namespace core::rendering
{
    class ForwardGeometryPass : public Subpass
    {
    public:
        ForwardGeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight);

        void subpass_init(SubpassShaderList& subpass_shaders, GPU_BufferContainer& buffer_container) override;
        void frame_pre_recording() override;
        void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index,
                             PushConstantBlock& push_constant_block,
                             SubpassShaderList& subpass_shaders,
                             GPU_BufferContainer& buffer_container, Vk_Image& depth_image) override;

        void cleanup() override;

    private:
        VkExtent2D extents{};

        std::vector<glm::vec4> cube;

        uint32_t cube_vertex_count = 0;

        void load_cube_model(const EngineContext& engine_context);
    };
}
