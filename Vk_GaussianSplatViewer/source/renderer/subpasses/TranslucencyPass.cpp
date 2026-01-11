#include "renderer/subpasses/TranslucencyPass.h"

#include <iostream>
#include <ostream>

#include "config/Config.inl"
#include "materials/MaterialUtils.h"
#include "structs/scene/PushConstantBlock.h"
#include "common/Types.h"


namespace core::rendering
{
    void TranslucencyPass::subpass_init(SubpassShaderList& subpass_shaders)
    {
        material::MaterialUtils material_utils(engine_context);
        subpass_shaders[ShaderObjectType::TranslucentPass] = material_utils.create_material("translucent_pass",
            shader_root_path + "/opaque/object.vert.spv", shader_root_path + "/translucency/color/oit_weighted_color.frag.spv");

        std::cout << subpass_shaders[ShaderObjectType::TranslucentPass] << std::endl;
    }

    void TranslucencyPass::frame_pre_recording(){}

    void TranslucencyPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders){ }

    void TranslucencyPass::cleanup(){ }
}