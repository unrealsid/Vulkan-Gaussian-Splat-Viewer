#pragma once

#include <memory>
#include <vector>

#include "renderer/RenderPass.h"
#include "structs/geometry/GaussianSurface.h"
#include "camera/FirstPersonCamera.h"

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

        template<typename V>
        void allocate_mesh_buffers(const std::vector<V>& vertices, const std::vector<uint32_t>& indices);

        void allocate_gaussian_buffer(const std::vector<GaussianSurface>& gaussians) const;
        void create_camera_buffer(uint32_t width, uint32_t height);

    private:
        EngineContext& engine_context;

        //How many images are we using for a single frame?
        uint32_t max_frames_in_flight;

        std::unique_ptr<camera::FirstPersonCamera> first_person_camera;

        std::unique_ptr<RenderPass> render_pass;

        void init_vulkan();

        void create_swapchain() const;
        void create_device() const;


        void cleanup();
    };
}
