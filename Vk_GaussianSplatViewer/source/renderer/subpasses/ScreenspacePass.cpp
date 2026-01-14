
#include "renderer/subpasses/ScreenspacePass.h"

#include <iostream>
#include <ostream>

#include "structs/scene/PushConstantBlock.h"
#include "common/Types.h"
#include "config/Config.inl"
#include "materials/MaterialUtils.h"
#include "structs/EngineContext.h"

namespace core::rendering
{
    ScreenspacePass::ScreenspacePass(EngineContext& engine_context, uint32_t max_frames_in_flight) : Subpass(engine_context, max_frames_in_flight){ }

    void ScreenspacePass::subpass_init(SubpassShaderList& subpass_shaders, GPU_BufferContainer& buffer_container)
    {
        setup_descriptor_layout();

        //Assign a material for this subpass and shaders
        material::MaterialUtils material_utils(engine_context);
        subpass_shaders[ShaderObjectType::ScreenspacePass] = material_utils.create_material("screenspace_pass",
            shader_root_path + "/translucency/composite/full_screen_triangle.vert.spv",
            shader_root_path + "/translucency/composite/oit_weighted_composite.frag.spv", &descriptor_set_layout, 1);

        std::cout << subpass_shaders[ShaderObjectType::ScreenspacePass] << std::endl;
    }

    void ScreenspacePass::render_target_init(EngineRenderTargets& render_targets){ }

    void ScreenspacePass::frame_pre_recording() { }

    void ScreenspacePass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index,
                                          PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders,
                                          GPU_BufferContainer& buffer_container,
                                          EngineRenderTargets& render_targets)
    {

    }

    void ScreenspacePass::setup_descriptor_layout()
    {
        VkDescriptorSetLayoutBinding bindings[] = {
            // Binding 0: Input attachment (accumulation)
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            },
            // Binding 1: Input attachment (revealage)
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 2;
        layoutInfo.pBindings = bindings;
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;

        engine_context.dispatch_table.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptor_set_layout);
    }

    void ScreenspacePass::push_descriptors(VkCommandBuffer cmd, VkPipelineLayout pipeline_layout,
        VkDescriptorType descriptor_type, uint32_t set_number, const VkImageView* image_views, uint32_t image_count, VkImageLayout image_layout, uint32_t first_binding) const
    {
        // Stack allocation for typical use cases (up to 8 descriptors)
        constexpr uint32_t MAX_DESCRIPTORS = 8;
        VkDescriptorImageInfo image_infos[MAX_DESCRIPTORS];
        VkWriteDescriptorSet writes[MAX_DESCRIPTORS];

        // Clamp to prevent buffer overflow
        image_count = image_count > MAX_DESCRIPTORS ? MAX_DESCRIPTORS : image_count;

        for (uint32_t i = 0; i < image_count; ++i)
        {
            // Setup image info
            image_infos[i].sampler = VK_NULL_HANDLE;
            image_infos[i].imageView = image_views[i];
            image_infos[i].imageLayout = image_layout;

            // Setup write descriptor
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[i].pNext = nullptr;
            writes[i].dstSet = VK_NULL_HANDLE;
            writes[i].dstBinding = first_binding + i;
            writes[i].dstArrayElement = 0;
            writes[i].descriptorCount = 1;
            writes[i].descriptorType = descriptor_type;
            writes[i].pImageInfo = &image_infos[i];
            writes[i].pBufferInfo = nullptr;
            writes[i].pTexelBufferView = nullptr;
        }

        // Push descriptors into command buffer
        engine_context.dispatch_table.cmdPushDescriptorSet(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            set_number,
            image_count,
            writes
        );
    }

    void ScreenspacePass::cleanup()
    {
        Subpass::cleanup();
    }
} // core