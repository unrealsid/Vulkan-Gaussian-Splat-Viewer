#include "renderer/subpasses/TranslucencyPass.h"

#include <iostream>
#include <ostream>

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

        std::cout << subpass_shaders[ShaderObjectType::TranslucentPass] << std::endl;
    }

    void TranslucencyPass::render_target_init(EngineRenderTargets& render_targets)
    {
        auto extents = render_targets.swapchain_extent;
        constexpr VmaAllocationCreateInfo alloc_info{};
        render_targets.accumulation_image = std::make_unique<Vk_Image>(utils::ImageUtils::create_image(engine_context, extents.width, extents.height, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,  alloc_info));
        render_targets.revealage_image = std::make_unique<Vk_Image>(utils::ImageUtils::create_image(engine_context, extents.width, extents.height, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,  alloc_info));
    }

    void TranslucencyPass::frame_pre_recording(){}

    void TranslucencyPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders, class
                                           GPU_BufferContainer& buffer_container, EngineRenderTargets& render_targets)
    {
        auto color_attachments = utils::RenderUtils::create_color_attachments(
        {
            {
                render_targets.accumulation_image->view,
                {0.0f, 0.0f, 0.0f, 1.0f},
                VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR
            },

            {
                render_targets.revealage_image->view,
                {0.0f, 0.0f, 0.0f, 1.0f},
                VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR
            }
        });//vector

        setup_depth_attachment(*render_targets.depth_stencil_image, { {1.0f, 0} }); //Clear depth

        auto image = render_targets.swapchain_images[image_index];

        begin_rendering(color_attachments);

        std::vector<VkColorComponentFlags> color_component_flags =
            {
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT,  // outColor

                VK_COLOR_COMPONENT_R_BIT   // outReveal
            };

        std::vector<VkBool32> color_blend_enables = {VK_FALSE, VK_FALSE};

        //Set initial state of render pass
        material::ShaderObject::set_initial_state(engine_context.dispatch_table, swapchain_manager->get_extent(),
                                                  *command_buffer,
                                                  GaussianSurfaceDescriptor::get_binding_descriptions(),
                                                  GaussianSurfaceDescriptor::get_attribute_descriptions(),
                                                  swapchain_manager->get_extent(),
                                                  {0, 0},
                                                  color_component_flags,
                                                  color_blend_enables );

        subpass_shaders[ShaderObjectType::OpaquePass]->get_shader_object()->bind_material_shader(engine_context.dispatch_table, *command_buffer);

        const auto* cube_buffer = buffer_container.get_buffer("cube_buffer");

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
