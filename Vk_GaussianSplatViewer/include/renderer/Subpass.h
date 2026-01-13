#pragma once

#include <memory>
#include <unordered_map>

#include "enums/PresentationImageType.h"
#include "materials/Material.h"
#include "platform/WindowManager.h"
#include "common/Types.h"
#include "structs/Vk_Image.h"

struct EngineRenderTargets;
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
    class GPU_BufferContainer;

    class Subpass
    {
    public:
        virtual ~Subpass() = default;
        explicit Subpass(EngineContext& engine_context, uint32_t max_frames_in_flight = 2);

        //Initializes the subpass and the associated material for the subpass if necessary
        virtual void subpass_init(SubpassShaderList& subpass_shaders,
                                  GPU_BufferContainer& buffer_container,
                                  EngineRenderTargets& render_targets) = 0;

        //Called before a frame is recorded
        virtual void frame_pre_recording() = 0;

        //Record commands into the buffer. Place draw commands here
        virtual void record_commands(VkCommandBuffer* command_buffer,
                                     uint32_t image_index,
                                     PushConstantBlock& push_constants,
                                     SubpassShaderList& subpass_shaders,
                                     GPU_BufferContainer& buffer_container,
                                     EngineRenderTargets& render_targets) = 0;

        //Cleanup pass
        virtual void cleanup();

        //Initialize parameters for a new frame. Runs at the start of every new frame
        void init_pass_new_frame(VkCommandBuffer p_command_buffer, uint32_t p_frame);

        void begin_command_buffer_recording() const;
        void setup_color_attachment(uint32_t image, VkClearValue clear_value);
        void setup_depth_attachment(const Vk_Image& depth_image, VkClearValue clear_value);
        void begin_rendering();
        void end_rendering();

    protected:
        EngineContext& engine_context;

        vulkanapp::SwapchainManager* swapchain_manager;
        vulkanapp::DeviceManager* device_manager;

        uint32_t max_frames_in_flight;

        //Which command buffer are we currently using?
        VkCommandBuffer active_command_buffer;
        uint32_t current_frame = 0;

        VkRenderingAttachmentInfoKHR color_attachment_info;
        VkRenderingAttachmentInfoKHR depth_attachment_info;
    };
}
