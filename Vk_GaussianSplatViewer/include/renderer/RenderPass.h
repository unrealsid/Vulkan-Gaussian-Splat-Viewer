#pragma once

#include "Subpass.h"
#include "structs/Vk_Image.h"

struct EngineContext;

namespace vulkanapp
{
    class SwapchainManager;
    class DeviceManager;
}

namespace core::renderer
{
    class RenderPass
    {
    public:
        RenderPass(EngineContext& engine_context, uint32_t max_frames_in_flight = 2);
        void allocate_and_record_command_buffers();
        void init_renderpass();
        void init_subpasses();

        [[nodiscard]] uint32_t get_max_frames_in_flight() const { return max_frames_in_flight; }

        VkCommandBuffer* get_command_buffer(uint32_t image_id);

        [[nodiscard]] VkCommandPool get_command_pool() const { return command_pool; }
        [[nodiscard]] Vk_Image* get_depth_stencil_image() const { return depth_stencil_image.get(); }

        void create_command_pool();
        void reset_command_pool();

        void allocate_command_buffer(uint32_t image);

        void create_depth_stencil_image();

        void reset_subpass_command_buffers();
        void recreate_swapchain();
        void create_rendering_resources();

        bool draw_frame(uint32_t image_index);

        void record_subpasses(uint32_t image_index);

        void record_commands_and_draw();
        void cleanup();

    private:
        std::vector<std::unique_ptr<Subpass>> subpasses;
        std::vector<VkCommandBuffer> command_buffers;

        vulkanapp::SwapchainManager* swapchain_manager{};
        vulkanapp::DeviceManager* device_manager{};

        std::vector<VkSemaphore> available_semaphores;
        std::vector<VkSemaphore> finished_semaphores;

        std::vector<VkFence> in_flight_fences;
        std::vector<VkFence> image_in_flight;

        EngineContext& engine_context;

        VkCommandPool command_pool;

        //Store created Depth Stencils
        std::unique_ptr<Vk_Image> depth_stencil_image;

        uint32_t max_frames_in_flight{};

        size_t current_frame = 0;

        bool create_sync_objects();
    };
}
