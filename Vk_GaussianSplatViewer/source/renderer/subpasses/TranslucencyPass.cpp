#include "renderer/subpasses/TranslucencyPass.h"

#include <iostream>
#include <ostream>

#include "config/Config.inl"
#include "materials/MaterialUtils.h"
#include "structs/scene/PushConstantBlock.h"
#include "common/Types.h"
#include "structs/EngineRenderTargets.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/MemoryUtils.h"


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


    }

    void TranslucencyPass::cleanup(){ }
}
