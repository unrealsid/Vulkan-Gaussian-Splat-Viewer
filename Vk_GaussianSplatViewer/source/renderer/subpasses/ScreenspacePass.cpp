
#include "renderer/subpasses/ScreenspacePass.h"

#include <iostream>
#include <ostream>

#include "structs/scene/PushConstantBlock.h"
#include "../../../include/common/Types.h"
#include "config/Config.inl"
#include "materials/MaterialUtils.h"

namespace core::rendering
{
    ScreenspacePass::ScreenspacePass(EngineContext& engine_context, uint32_t max_frames_in_flight) : Subpass(engine_context, max_frames_in_flight){ }

    void ScreenspacePass::subpass_init(SubpassShaderList& subpass_shaders)
    {
        //Assign a material for this subpass and shaders
        material::MaterialUtils material_utils(engine_context);
        subpass_shaders[ShaderObjectType::ScreenspacePass] = material_utils.create_material("screenspace_pass",
            shader_root_path + "/translucency/composite/full_screen_triangle.vert.spv",
            shader_root_path + "/translucency/composite/oit_weighted_composite.frag.spv");

        std::cout << subpass_shaders[ShaderObjectType::ScreenspacePass] << std::endl;
    }

    void ScreenspacePass::frame_pre_recording()
    {
    }

    void ScreenspacePass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index,
        PushConstantBlock& push_constant_block, SubpassShaderList& subpass_shaders)
    {

    }

    void ScreenspacePass::cleanup()
    {
        Subpass::cleanup();
    }
} // core