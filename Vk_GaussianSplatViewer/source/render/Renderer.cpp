#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "materials/Material.h"
#include "renderer/RenderPass.h"
#include "structs/EngineContext.h"

namespace core::renderer
{
    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();

        render_pass = std::make_unique<RenderPass>(engine_context, 2);
        render_pass->init_renderpass();

        std::cout << "Done";
    }

    void Renderer::renderer_update()
    {
        std::cout << "Renderer Update" << std::endl;

        //TODO: Change this.
        if (engine_context.start_rendering)
        {
            while (true)
            {
                render_pass->record_commands_and_draw();
            }
        }
    }
}
