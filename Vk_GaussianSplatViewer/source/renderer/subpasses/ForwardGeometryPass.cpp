#include "renderer/subpasses/ForwardGeometryPass.h"

#include "3d/ModelUtils.h"
#include "config/Config.inl"
#include "materials/MaterialUtils.h"
#include "structs/EngineContext.h"
#include "structs//geometry/Vertex.h"
#include "structs/scene/PushConstantBlock.h"
#include "enums/inputs/UIAction.h"
#include "vulkanapp/VulkanCleanupQueue.h"
#include "vulkanapp/utils/MemoryUtils.h"
#include "renderer/GPU_BufferContainer.h"
#include "common/Types.h"

namespace core::rendering
{
    ForwardGeometryPass::ForwardGeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight) : Subpass(engine_context, max_frames_in_flight){}

    void ForwardGeometryPass::subpass_init(SubpassShaderList& subpass_shaders, GPU_BufferContainer& buffer_container)
    {
        //Assign a material for this subpass and shaders
        material::MaterialUtils material_utils(engine_context);
        subpass_shaders[ShaderObjectType::OpaquePass] = material_utils.create_material("opaque_pass",
            shader_root_path + "/opaque/object.vert.spv",
            shader_root_path + "/opaque/opaque.frag.spv");

        extents = swapchain_manager->get_extent();

        load_cube_model(engine_context);

        //Placeholder scene initialization
        auto gaussian_surfaces = std::vector<glm::vec4>{ {0.0, 0.0, 0.0, 1.0} };
        auto alphas = std::vector<float>{ 0.0 };

        buffer_container.allocate_gaussian_buffer("positions", gaussian_surfaces);
        buffer_container.allocate_gaussian_buffer("scales", gaussian_surfaces);
        buffer_container.allocate_gaussian_buffer("colors", gaussian_surfaces);
        buffer_container.allocate_gaussian_buffer("quaternions", gaussian_surfaces);
        buffer_container.allocate_gaussian_buffer("alpha", alphas);
        buffer_container.gaussian_count = 1;

        //Register a new event to allocate memory when a new model is loaded
        engine_context.ui_action_manager->register_string_action(UIAction::ALLOCATE_SPLAT_MEMORY,
             [this, &buffer_container](const std::string& code)
             {
                 engine_context.dispatch_table.deviceWaitIdle();

                 std::string str = R"(D:\Data\point_cloud_truck_30k.ply)";
                 auto gaussian_surfaces = entity_3d::ModelUtils::load_gaussian_surfaces(str);
                 //entity_3d::ModelUtils::load_placeholder_gaussian_model();

                 const auto& positions = gaussian_surfaces.get_positions();
                 const auto& scales = gaussian_surfaces.get_scales();
                 const auto& colors = gaussian_surfaces.get_colors();
                 const auto& quaternions = gaussian_surfaces.get_quaternions();
                 const auto& alpha = gaussian_surfaces.get_alphas();

                 buffer_container.allocate_gaussian_buffer("positions", positions);
                 buffer_container.allocate_gaussian_buffer("scales", scales);
                 buffer_container.allocate_gaussian_buffer("colors", colors);
                 buffer_container.allocate_gaussian_buffer("quaternions", quaternions);
                 buffer_container.allocate_gaussian_buffer("alpha", alpha);

                 buffer_container.gaussian_count = gaussian_surfaces.get_count();
             });
    }

    void ForwardGeometryPass::load_cube_model(const EngineContext& engine_context)
    {
        auto buffer_container = engine_context.buffer_container.get();
        cube =  entity_3d::ModelUtils::load_gaussian_bounding_box();
        cube_vertex_count = cube.size();
        buffer_container->allocate_gaussian_buffer("cube_buffer", cube);
    }


    void ForwardGeometryPass::frame_pre_recording(){ }

    void ForwardGeometryPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index,
                                              PushConstantBlock& push_constant_block,
                                              SubpassShaderList& subpass_shaders,
                                              GPU_BufferContainer& buffer_container,
                                              Vk_Image& depth_image)
    {
        const auto* cube_buffer = buffer_container.get_buffer("cube_buffer");
        const auto* positions = buffer_container.get_buffer("positions");
        const auto* scales = buffer_container.get_buffer("scales");
        const auto* colors = buffer_container.get_buffer("colors");
        const auto* quaternions = buffer_container.get_buffer("quaternions");
        const auto* alphas = buffer_container.get_buffer("alpha");

        setup_color_attachment(image_index, { {0.0f, 0.0f, 0.0f, 1.0f} });
        setup_depth_attachment(depth_image, { {1.0f, 0} }); //Clear depth

        begin_rendering();

        //Set initial state of render pass
        material::ShaderObject::set_initial_state(engine_context.dispatch_table, swapchain_manager->get_extent(),
                                                                            *command_buffer,
                                                                            GaussianSurfaceDescriptor::get_binding_descriptions(),
                                                                            GaussianSurfaceDescriptor::get_attribute_descriptions(),
                                                                            swapchain_manager->get_extent(),
                                                                            {0, 0});

        subpass_shaders[ShaderObjectType::OpaquePass]->get_shader_object()->bind_material_shader(engine_context.dispatch_table, *command_buffer);

        //Vertices
        VkBuffer vertex_buffers[] = { cube_buffer->buffer};
        VkDeviceSize offsets[] = {0};
        engine_context.dispatch_table.cmdBindVertexBuffers(*command_buffer, 0, 1, vertex_buffers, offsets);

        //Push Constants
        push_constant_block = {
            .scene_buffer_address = buffer_container.camera_data_buffer.buffer_address,
        };
        engine_context.dispatch_table.cmdPushConstants(*command_buffer, subpass_shaders[ShaderObjectType::OpaquePass]->get_pipeline_layout(),
                                                                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                0, sizeof(PushConstantBlock), &push_constant_block);

        engine_context.dispatch_table.cmdDraw(*command_buffer, cube_vertex_count, buffer_container.gaussian_count, 0, 0);

        end_rendering();
    }

    void ForwardGeometryPass::cleanup()
    {
        Subpass::cleanup();

        engine_context.buffer_container->destroy_buffers();
    }
}
