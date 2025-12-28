#pragma once

#include "renderer/Subpass.h"

namespace core::renderer
{
    class GeometryPass : public Subpass
    {

    public:
        GeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight);

        void record_commands(VkCommandBuffer* command_buffer) override;
    };
}