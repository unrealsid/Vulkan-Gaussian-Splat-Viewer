#pragma once

#include <memory>
#include <unordered_map>

#include "enums/PresentationImageType.h"
#include "materials/Material.h"
#include "platform/WindowManager.h"
#include "../common/Types.h"
#include "structs/Vk_Image.h"

struct PushConstantBlock;
struct EngineContext;
struct WindowCreateParams;

namespace vulkanapp
{
    class SwapchainManager;
    class DeviceManager;
}

namespace core::rendering
{
    class Subpass
    {
    public:
        virtual ~Subpass() = default;
        explicit Subpass(EngineContext& engine_context, uint32_t max_frames_in_flight = 2);

        //Initializes the subpass and the associated material for the subpass if necessary
        virtual void subpass_init(SubpassShaderList& subpass_shaders) = 0;

        //Called before a frame is recorded
        virtual void frame_pre_recording() = 0;

        //Record commands into the buffer. Place draw commands here
        virtual void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, PushConstantBlock& push_constants, SubpassShaderList& subpass_shaders) = 0;

        //Cleanup pass
        virtual void cleanup();

        //Initialize parameters for a new frame. Runs at the start of every new frame
        void init_pass_new_frame(VkCommandBuffer p_command_buffer, Vk_Image* p_depth_stencil_image, uint32_t p_frame);

        void begin_command_buffer_recording() const;
        void set_present_image_transition(uint32_t image_id, PresentationImageType presentation_image_type) const;
        void setup_color_attachment(uint32_t image, VkClearValue clear_value);
        void setup_depth_attachment(VkClearValue clear_value);
        void begin_rendering();
        void end_rendering();
        void end_command_buffer_recording(uint32_t image) const;

        void set_material(const std::shared_ptr<material::Material>& material);

    protected:
        EngineContext& engine_context;

        vulkanapp::SwapchainManager* swapchain_manager;
        vulkanapp::DeviceManager* device_manager;

        uint32_t max_frames_in_flight;

        //Which command buffer are we currently using?
        VkCommandBuffer active_command_buffer;
        Vk_Image* depth_stencil_image;
        uint32_t current_frame = 0;

        VkRenderingAttachmentInfoKHR color_attachment_info;
        VkRenderingAttachmentInfoKHR depth_attachment_info;
    };
}
