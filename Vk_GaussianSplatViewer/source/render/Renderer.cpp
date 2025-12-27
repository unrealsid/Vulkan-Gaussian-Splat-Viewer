#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "Config.inl"
#include "materials/Material.h"
#include "renderer/Subpass.h"
#include "structs//PushConstantBlock.h"
#include "structs/Vertex.h"

namespace core::renderer
{
    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();
        init_cleanup();

        render_context->render_pass = std::make_unique<RenderPass>(render_context.get(), engine_context, 2);
        render_context->render_pass->init_renderpass();

        std::cout << "Done";
    }

    void Renderer::renderer_update()
    {
        std::cout << "\nFrame Context can render: " << engine_context->start_rendering;
        std::cout << "Renderer Update" << std::endl;

        //TODO: Change this.
        if (engine_context->start_rendering)
        {
            while (true)
            {
                process_command();
                render_context->render_pass->record_commands_and_draw();
            }
        }
    }

    void Renderer::process_command()
    {
        auto command = engine_context->get_next_command();

        switch (command.command)
        {
        case RenderCommand::None:
            break;
        case RenderCommand::AllocateMeshBuffers:
            allocate_mesh_buffers(command);
            break;
        case RenderCommand::TransferEntityData:
            transfer_entity_data(command);
            break;
        }
    }
}
