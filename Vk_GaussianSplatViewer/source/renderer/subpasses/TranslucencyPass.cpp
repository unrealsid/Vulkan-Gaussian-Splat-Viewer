#include "renderer/subpasses/TranslucencyPass.h"

#include "config/Config.inl"
#include "materials/MaterialUtils.h"
#include "structs/scene/PushConstantBlock.h"
#include "common/Types.h"
#include "structs/EngineContext.h"
#include "structs/EngineRenderTargets.h"
#include "structs/geometry/GaussianSurface.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/RenderUtils.h"


namespace core::rendering
{
    void TranslucencyPass::subpass_init(SubpassShaderList& subpass_shaders, GPU_BufferContainer& buffer_container)
    {
        material::MaterialUtils material_utils(engine_context);
        subpass_shaders[ShaderObjectType::TranslucentPass] = material_utils.create_material("translucent_pass",
            shader_root_path + "/opaque/object.vert.spv", shader_root_path + "/translucency/color/oit_weighted_color.frag.spv", nullptr, 0);
    }

    void TranslucencyPass::render_target_init(EngineRenderTargets& render_targets)
    {
        auto extents = render_targets.swapchain_extent;
        constexpr VmaAllocationCreateInfo alloc_info{};
        render_targets.accumulation_image = std::make_unique<Vk_Image>(utils::ImageUtils::create_image(engine_context, extents.width, extents.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,  alloc_info));
        render_targets.revealage_image = std::make_unique<Vk_Image>(utils::ImageUtils::create_image(engine_context, extents.width, extents.height, VK_FORMAT_R16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,  alloc_info));
    }

    void TranslucencyPass::frame_pre_recording(){}

    void TranslucencyPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders, class
                                           GPU_BufferContainer& buffer_container, EngineRenderTargets& render_targets)
    {
        auto color_attachments = utils::RenderUtils::create_color_attachments(
        {
            {
                render_targets.accumulation_image->view,
                {0.0f, 0.0f, 0.0f, 0.0f},
                VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR
            },

            {
                render_targets.revealage_image->view,
                {1.0f, 0.0f, 0.0f, 0.0f},
                VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR
            }
        });//vector

        utils::ImageUtils::image_layout_transition(*command_buffer, render_targets.accumulation_image->image,
                                                VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                                VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                0,
                                                VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                                VK_IMAGE_LAYOUT_UNDEFINED,
                                                VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ,
                                                VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        //Set Layout transition for depth image
        utils::ImageUtils::image_layout_transition(*command_buffer,  render_targets.revealage_image->image,
                                                     VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                                     VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                     0,
                                                     VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                                     VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ,
                                                      VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        //We only need to use depth testing for comparision.
        //Writing is already done in the opaque geometry pass
        setup_depth_attachment(
       {
           render_targets.depth_stencil_image->view,
           {0.0f},
           VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
           VK_ATTACHMENT_LOAD_OP_LOAD,
           VK_ATTACHMENT_STORE_OP_STORE
       });

         begin_rendering(color_attachments);

         std::vector<VkColorComponentFlags> color_component_flags =
             {
                 VK_COLOR_COMPONENT_R_BIT |
                 VK_COLOR_COMPONENT_G_BIT |
                 VK_COLOR_COMPONENT_B_BIT |
                 VK_COLOR_COMPONENT_A_BIT,  // outColor

                VK_COLOR_COMPONENT_R_BIT
             };

         std::vector color_blend_enables = {VK_TRUE, VK_TRUE};

         draw_state->set_and_apply_viewport_scissor(*command_buffer, swapchain_manager->get_extent(), swapchain_manager->get_extent(), {0, 0});
         draw_state->set_and_apply_color_blend(*command_buffer, color_component_flags, color_blend_enables);

         //Set blend equation for attachment 0
         draw_state->set_blend_equation(
             0,              // srcColorBlend
             // Color blend factors (for accumulation attachment 0)
             VK_BLEND_FACTOR_ONE,              // dstColorBlend
             VK_BLEND_FACTOR_ONE,                  // colorBlendOp
             VK_BLEND_OP_ADD,              // srcAlphaBlend
             // Alpha blend factors (for accumulation attachment 0)
             VK_BLEND_FACTOR_ONE,              // dstAlphaBlend
             VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD // alphaBlendOp
         );

         //Set blend equation for attachment 1
         draw_state->set_blend_equation(
             1,
             VK_BLEND_FACTOR_ZERO,
             VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
             VK_BLEND_OP_ADD,
             VK_BLEND_FACTOR_ZERO,
             VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR, VK_BLEND_OP_ADD
         );

         draw_state->apply_blend_equation(*command_buffer);


         draw_state->apply_rasterization_state(*command_buffer);
         draw_state->apply_depth_stencil_state(*command_buffer, VK_FALSE, VK_FALSE);

         draw_state->set_and_apply_vertex_input(*command_buffer, GaussianSurfaceDescriptor::get_binding_descriptions(), GaussianSurfaceDescriptor::get_attribute_descriptions());

         subpass_shaders[ShaderObjectType::TranslucentPass]->get_shader_object()->bind_material_shader(engine_context.dispatch_table, *command_buffer);

          const auto* cube_buffer = buffer_container.get_buffer("cube_buffer");
          const auto* cube_color_buffer = buffer_container.get_buffer("cube_color_buffer");

          //Vertices
          VkBuffer vertex_buffers[] = { cube_buffer->buffer, cube_color_buffer->buffer};
          VkDeviceSize offsets[] = {0, 0};
          engine_context.dispatch_table.cmdBindVertexBuffers(*command_buffer, 0, 2, vertex_buffers, offsets);

          //Push Constants
          push_constant_block = {
              .scene_buffer_address = buffer_container.camera_data_buffer.buffer_address,
          };
          engine_context.dispatch_table.cmdPushConstants(*command_buffer, subpass_shaders[ShaderObjectType::TranslucentPass]->get_pipeline_layout(),
                                                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                  0, sizeof(PushConstantBlock), &push_constant_block);

          engine_context.dispatch_table.cmdDraw(*command_buffer, 36, buffer_container.gaussian_count, 0, 0);

         //Add a barrier so the next stage can read the pass
          VkMemoryBarrier2KHR memoryBarrier{};
          memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
          memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
          memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
          memoryBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
          memoryBarrier.dstAccessMask = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;

          VkDependencyInfoKHR dependencyInfo{};
          dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
          dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
          dependencyInfo.memoryBarrierCount = 1;
          dependencyInfo.pMemoryBarriers = &memoryBarrier;

         engine_context.dispatch_table.cmdPipelineBarrier2(*command_buffer, &dependencyInfo);

        end_rendering();
    }

    void TranslucencyPass::cleanup(){ }
}
