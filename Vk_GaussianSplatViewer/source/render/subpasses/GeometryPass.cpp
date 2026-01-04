#include "renderer/subpasses/GeometryPass.h"

#include "3d/ModelUtils.h"
#include "materials/MaterialUtils.h"
#include "structs/EngineContext.h"
#include "structs//geometry/Vertex.h"
#include "structs/scene/CameraData.h"
#include "structs/scene/PushConstantBlock.h"
#include "enums/inputs/UIAction.h"
#include "vulkanapp/VulkanCleanupQueue.h"
#include "vulkanapp/utils/MemoryUtils.h"
#include "renderer/GPU_BufferContainer.h"

namespace core::renderer
{
    GeometryPass::GeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight) : Subpass(engine_context, max_frames_in_flight)
    {
        material::MaterialUtils material_utils(engine_context);
        set_material(material_utils.create_material("default"));

        camera_data = {glm::mat4{}, glm::mat4{}};
        camera = engine_context.renderer->get_camera();
        extents = swapchain_manager->get_extent();

        buffer_container = engine_context.buffer_container.get();

        auto gaussian_surfaces = entity_3d::ModelUtils::load_placeholder_gaussian_model();
        buffer_container->allocate_gaussian_surface_buffer(gaussian_surfaces);

        //Register new event to allocate memory when a new model is loaded
        engine_context.ui_action_manager->register_string_action(UIAction::ALLOCATE_SPLAT_MEMORY,
             [this, &engine_context](const std::string& code)
             {
                engine_context.dispatch_table.deviceWaitIdle();

                //std::string str = R"(D:\Projects\CPP\Vk_GaussianSplat\data\point_cloud_truck_30k.ply)";
                auto gaussian_surfaces = entity_3d::ModelUtils::load_gaussian_surfaces(code);
                //entity_3d::ModelUtils::load_placeholder_gaussian_model();
                buffer_container->allocate_gaussian_surface_buffer(gaussian_surfaces);
                buffer_container->gaussian_count = gaussian_surfaces.size();
             });
    }

    void GeometryPass::frame_pre_recording()
    {

    }

    void GeometryPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, bool is_last)
    {
        set_present_image_transition(image_index, PresentationImageType::SwapChain);
        set_present_image_transition(current_frame, PresentationImageType::DepthStencil);
        setup_color_attachment(image_index, { {0.0f, 0.0f, 0.0f, 1.0f} });
        setup_depth_attachment({ {1.0f, 0} });

        begin_rendering();

        material::ShaderObject::set_initial_state(engine_context.dispatch_table, swapchain_manager->get_extent(), *command_buffer,
                                                                            GaussianSurfaceDescriptor::get_binding_description(),
                                                                            GaussianSurfaceDescriptor::get_attribute_descriptions(),
                                                                            swapchain_manager->get_extent(), {0, 0});

        material_to_use->get_shader_object()->bind_material_shader(engine_context.dispatch_table, *command_buffer);

        camera_data.projection =  camera->get_projection_matrix();
        camera_data.view = camera->get_view_matrix();

        memcpy(buffer_container->camera_data_buffer.allocation_info.pMappedData, &camera_data, sizeof(CameraData));

        //Vertices
        VkBuffer vertex_buffers[] = {buffer_container->gaussian_buffer.buffer};
        VkDeviceSize offsets[] = {0};
        engine_context.dispatch_table.cmdBindVertexBuffers(*command_buffer, 0, 1, vertex_buffers, offsets);

        //Push Constants
        PushConstantBlock push_constant_block = {buffer_container->camera_data_buffer.buffer_address};
        engine_context.dispatch_table.cmdPushConstants(*command_buffer, material_to_use->get_pipeline_layout(),  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0, sizeof(PushConstantBlock), &push_constant_block);

        engine_context.dispatch_table.cmdDraw(*command_buffer, buffer_container->gaussian_count, 1, 0, 0);

        end_rendering();
        end_command_buffer_recording(image_index, is_last);
    }

    void GeometryPass::cleanup()
    {
        Subpass::cleanup();

        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), buffer_container->mesh_vertices_buffer);
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), buffer_container->mesh_indices_buffer);
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), buffer_container->gaussian_buffer);
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), buffer_container->camera_data_buffer);
    }
}
