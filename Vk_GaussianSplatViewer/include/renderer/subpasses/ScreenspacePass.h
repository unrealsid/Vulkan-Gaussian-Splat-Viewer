#pragma once
#include "renderer/Subpass.h"

namespace core::rendering
{
    class ScreenspacePass : public Subpass
    {
    public:
        ScreenspacePass(EngineContext& engine_context, uint32_t max_frames_in_flight);

        void subpass_init(SubpassShaderList& subpass_shaders, GPU_BufferContainer& buffer_container) override;
        void render_target_init(EngineRenderTargets& render_targets) override;
        void frame_pre_recording() override;
        void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders, class
                             GPU_BufferContainer& buffer_container, EngineRenderTargets& render_targets) override ;
        void cleanup() override;
    };
}
