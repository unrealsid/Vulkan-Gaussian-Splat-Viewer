#include "renderer/subpasses/GeometryPass.h"
#include "materials/MaterialUtils.h"
#include "structs/EngineContext.h"
#include "structs/Vertex.h"
#include "structs/Vertex2D.h"

namespace core::renderer
{
    GeometryPass::GeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight): Subpass(engine_context, max_frames_in_flight)
    {
        material::MaterialUtils material_utils(engine_context);
        set_material(material_utils.create_material("default"));
    }

    void GeometryPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index)
    {
        engine_context.dispatch_table.resetCommandBuffer(*command_buffer, 0);

        begin_command_buffer_recording();
        set_present_image_transition(image_index, PresentationImageType::SwapChain);
        set_present_image_transition(current_frame, PresentationImageType::DepthStencil);
        setup_color_attachment(image_index, { {0.1f, 0.1f, 0.1f, 1.0f} });
        setup_depth_attachment({ {1.0f, 0} });

        begin_rendering();

        material::ShaderObject::set_initial_state(engine_context.dispatch_table, swapchain_manager->get_extent(), *command_buffer,
                                                                            Vertex2D::get_binding_description(), Vertex2D::get_attribute_descriptions(),
                                                                            swapchain_manager->get_extent(), {0, 0});

        material_to_use->get_shader_object()->bind_material_shader(engine_context.dispatch_table, *command_buffer);

        VkBuffer vertex_buffers[] = {engine_context.mesh_vertices_buffer.buffer};

        VkDeviceSize offsets[] = {0};
        engine_context.dispatch_table.cmdBindVertexBuffers(*command_buffer, 0, 1, vertex_buffers, offsets);

        engine_context.dispatch_table.cmdDraw(*command_buffer, 3, 1, 0, 0);

        end_rendering();
        end_command_buffer_recording(image_index);
    }
 }
