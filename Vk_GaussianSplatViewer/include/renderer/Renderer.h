#pragma once

#include <memory>
#include <utility>

#include "platform/WindowManager.h"
#include "structs/EngineContext.h"
#include "structs/RenderContext.h"

struct RenderContext;
struct WindowCreateParams;

namespace core::renderer
{
    class Renderer
    {
    public:
        explicit Renderer(std::shared_ptr<EngineContext> engine_context, const uint32_t p_max_frames_in_flight = 2) :
                                                                        engine_context(std::move(engine_context)),
                                                                        max_frames_in_flight(p_max_frames_in_flight)
        {
        }

        void renderer_init();
        void renderer_update();

        [[nodiscard]] RenderContext* get_render_context() const
        {
            return render_context.get();
        }

    private:
        std::unique_ptr<RenderContext> render_context;
        std::shared_ptr<EngineContext> engine_context;

        //How many images are we using for a single frame?
        uint32_t max_frames_in_flight;

        void init_vulkan();

        void create_swapchain() const;
        void create_device() const;
        void init_cleanup() const;

        void process_command();

        void cleanup();
    };
}
