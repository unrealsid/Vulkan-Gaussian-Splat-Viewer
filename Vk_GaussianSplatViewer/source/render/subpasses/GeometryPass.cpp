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

namespace core::renderer
{
    GeometryPass::GeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight, CommonSceneData& common_scene_data) :
    Subpass(engine_context, max_frames_in_flight), common_scene_data(common_scene_data)
    {
        material::MaterialUtils material_utils(engine_context);
        set_material(material_utils.create_material("default"));

        camera_data = {glm::mat4{}, glm::mat4{}};
        camera = engine_context.renderer->get_camera();
        extents = swapchain_manager->get_extent();

        geometry_data = std::make_unique<GaussianGeometryData>();

        auto gaussian_surfaces = entity_3d::ModelUtils::load_placeholder_gaussian_model();
        allocate_gaussian_surface_buffer(gaussian_surfaces);

        //Register new event to allocate memory when a new model is loaded
        engine_context.ui_action_manager->register_string_action(UIAction::ALLOCATE_SPLAT_MEMORY,
             [this](const std::string& code)
             {
                 std::string str = R"(D:\Projects\CPP\Vk_GaussianSplat\data\point_cloud_truck_30k.ply)";
                 auto gaussian_surfaces = entity_3d::ModelUtils::load_gaussian_surfaces(code);
                 allocate_gaussian_surface_buffer(gaussian_surfaces);
                 geometry_data->gaussian_count = gaussian_surfaces.size();
             });
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

        memcpy(common_scene_data.camera_data_buffer.allocation_info.pMappedData, &camera_data, sizeof(CameraData));

        //Vertices
        VkBuffer vertex_buffers[] = {geometry_data->gaussian_buffer.buffer};
        VkDeviceSize offsets[] = {0};
        engine_context.dispatch_table.cmdBindVertexBuffers(*command_buffer, 0, 1, vertex_buffers, offsets);

        //Push Constants
        PushConstantBlock push_constant_block = {common_scene_data.camera_data_buffer.buffer_address};
        engine_context.dispatch_table.cmdPushConstants(*command_buffer, material_to_use->get_pipeline_layout(),  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0, sizeof(PushConstantBlock), &push_constant_block);

        engine_context.dispatch_table.cmdDraw(*command_buffer, geometry_data->gaussian_count, 1, 0, 0);

        end_rendering();
        end_command_buffer_recording(image_index, is_last);
    }

    void GeometryPass::allocate_gaussian_surface_buffer(const std::vector<GaussianSurface>& gaussians) const
    {
        vmaDestroyBuffer(device_manager->get_allocator(), geometry_data->gaussian_buffer.buffer, geometry_data->gaussian_buffer.allocation);
        geometry_data->gaussian_buffer = {};

        utils::MemoryUtils::create_vertex_buffer_with_staging(engine_context,
                                                              gaussians,
                                                              engine_context.renderer->get_render_pass()->get_command_pool(),
                                                              geometry_data->gaussian_buffer);
    }

    void GeometryPass::cleanup()
    {
        Subpass::cleanup();

        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), geometry_data->mesh_vertices_buffer);
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), geometry_data->mesh_indices_buffer);
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), geometry_data->gaussian_buffer);
        utils::MemoryUtils::destroy_buffer(engine_context.device_manager->get_allocator(), common_scene_data.camera_data_buffer);
    }
}
