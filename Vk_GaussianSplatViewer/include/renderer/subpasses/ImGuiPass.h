#pragma once

#include <vulkan/vulkan_core.h>
#include "renderer/Subpass.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

namespace core::renderer
{
    class ImGuiPass : public Subpass
    {
    public:
        ImGuiPass(EngineContext& engine_context, uint32_t max_frames_in_flight);
        ~ImGuiPass() override;

        void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, bool is_last) override;
        void cleanup() override;

    private:
        void init_imgui();
        VkDescriptorPool imgui_pool{};
    };
}
