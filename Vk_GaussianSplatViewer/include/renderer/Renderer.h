#pragma once

#include <cstdint>
#include <vector>

struct EngineContext;
struct WindowCreateParams;

namespace core::renderer
{
    class Renderer
    {
    public:
        explicit Renderer(EngineContext& engine_context, const uint32_t p_max_frames_in_flight = 2) :
                                                                        engine_context(engine_context),
                                                                        max_frames_in_flight(p_max_frames_in_flight)
        {
        }

        void renderer_init();
        void renderer_update();

    private:
        EngineContext& engine_context;

        //How many images are we using for a single frame?
        uint32_t max_frames_in_flight;

        void init_vulkan();

        void create_swapchain() const;
        void create_device() const;
        void init_cleanup() const;

        void cleanup();

        template<typename V>
        void allocate_mesh_buffers(const std::vector<V>& vertices, const std::vector<uint32_t>& indices);
    };
}
