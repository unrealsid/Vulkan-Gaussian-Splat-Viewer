#pragma once

#include "renderer/Subpass.h"

namespace core::rendering
{
    class TranslucencyPass : public Subpass
    {
    public:
        TranslucencyPass(EngineContext& engine_context, uint32_t max_frames_in_flight)
            : Subpass(engine_context, max_frames_in_flight){}

            void subpass_init(SubpassShaderList& subpass_shaders) override;
            void frame_pre_recording() override;
            void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders) override;
            void cleanup() override;
    };
} // rendering