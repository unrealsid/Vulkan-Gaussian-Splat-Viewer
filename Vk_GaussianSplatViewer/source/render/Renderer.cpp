#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "config/Config.inl"
#include "renderer/RenderPass.h"
#include "structs/EngineContext.h"

namespace core::renderer
{
    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();

        create_camera_buffer(window_width, window_height);
        render_pass = std::make_unique<RenderPass>(engine_context, 2);
        render_pass->init_renderpass();


        std::cout << "Done initializing the renderer";
    }

    void Renderer::renderer_update()
    {
        render_pass->record_commands_and_draw();
    }
}
