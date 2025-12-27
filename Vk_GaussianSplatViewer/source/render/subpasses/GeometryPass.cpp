#include "renderer/subpasses/GeometryPass.h"

#include "materials/MaterialUtils.h"
#include "structs/Vertex.h"

namespace core::renderer
{
    GeometryPass::GeometryPass(RenderContext* render_context, uint32_t max_frames_in_flight): Subpass(render_context, max_frames_in_flight)
    {
        material::MaterialUtils material_utils(render_context);
        set_material(material_utils.create_material("default"));
    }

    void GeometryPass::record_commands(VkCommandBuffer* command_buffer)
    {
        render_context->dispatch_table.resetCommandBuffer(*command_buffer, 0);

        begin_command_buffer_recording();
        set_present_image_transition(current_frame, PresentationImageType::SwapChain);
        set_present_image_transition(current_frame, PresentationImageType::DepthStencil);
        setup_color_attachment(current_frame, { {0.1f, 0.1f, 0.1f, 1.0f} });
        setup_depth_attachment({ {1.0f, 0} });

        begin_rendering();

        material::ShaderObject::set_initial_state(render_context->dispatch_table, swapchain_manager->get_extent(), *command_buffer,
                                                                            Vertex2D::get_binding_description(), Vertex2D::get_attribute_descriptions(),
                                                                            swapchain_manager->get_extent(), {0, 0});

        material_to_use->get_shader_object()->bind_material_shader(render_context->dispatch_table, *command_buffer);

        VkBuffer vertex_buffers[] = {render_context->mesh_vertices_buffer.buffer};

        VkDeviceSize offsets[] = {0};
        render_context->dispatch_table.cmdBindVertexBuffers(*command_buffer, 0, 1, vertex_buffers, offsets);

        render_context->dispatch_table.cmdDraw(*command_buffer, 3, 1, 0, 0);

        end_rendering();
        end_command_buffer_recording(current_frame);
    }
 }
