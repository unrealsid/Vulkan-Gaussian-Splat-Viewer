#pragma once

#include "Subpass.h"
#include "camera/FirstPersonCamera.h"
#include "structs/Vk_Image.h"
#include "common/Types.h"
#include "structs/scene/CameraData.h"
struct EngineContext;

namespace vulkanapp
{
    class SwapchainManager;
    class DeviceManager;
}

namespace core::rendering
{
    class GPU_BufferContainer;

    class RenderPass
    {
    public:
        RenderPass(EngineContext& engine_context, uint32_t max_frames_in_flight = 2);
        void allocate_and_record_command_buffers();
        void renderpass_init();

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
        void create_renderpass_resources(bool is_init);

        bool draw_frame(uint32_t image_index);

        void record_subpasses(uint32_t image_index);
        void recreate_render_resources();

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

        GPU_BufferContainer* common_scene_data;

        //Store created Depth Stencils
        std::unique_ptr<Vk_Image> depth_stencil_image;

        uint32_t max_frames_in_flight{};

        size_t current_frame = 0;

        //Stores camera data on the cpu
        CameraData camera_data{};

        //References a camera
        camera::FirstPersonCamera* camera;

        //Subpass dependent shader objects
        //Individual subpasses initialize their own subpass shader objects if needed
        SubpassShaderList subpass_shader_objects;

        //Stores a reference to the buffer container
        GPU_BufferContainer* buffer_container;

        bool create_sync_objects();
        void set_new_camera_aspect_ratio() const;
        void map_camera_data();

        //Maps cpu -> gpu data
        void map_cpu_data();

        void set_present_image_transition(uint32_t image_id, VkCommandBuffer command_buffer, PresentationImageType presentation_image_type) const;
        void finish_image_transition_recording(uint32_t image, VkCommandBuffer command_buffer) const;
    };
}
