#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "renderer/RenderPass.h"
#include "camera/FirstPersonCamera.h"
#include "structs/scene/CommonSceneData.h"

struct EngineContext;
struct WindowCreateParams;

namespace core::renderer
{
    class Renderer
    {
    public:
        explicit Renderer(EngineContext& engine_context, const uint32_t p_max_frames_in_flight = 2) :
                                                                        engine_context(engine_context),
                                                                        max_frames_in_flight(p_max_frames_in_flight),
                                                                        render_pass(nullptr)
        {
        }

        [[nodiscard]] camera::FirstPersonCamera* get_camera() const { return first_person_camera.get(); }

        void renderer_init();
        void renderer_update();

        void init_cleanup() const;

        [[nodiscard]] RenderPass* get_render_pass() const { return render_pass.get(); }

        void create_camera_buffer(uint32_t width, uint32_t height);

    private:
        EngineContext& engine_context;

        //How many images are we using for a single frame?
        uint32_t max_frames_in_flight;

        std::unique_ptr<camera::FirstPersonCamera> first_person_camera;

        std::unique_ptr<RenderPass> render_pass;
        std::unique_ptr<CommonSceneData> common_scene_data;

        void init_vulkan();

        void create_swapchain() const;
        void create_device() const;


        void cleanup();
    };
}
