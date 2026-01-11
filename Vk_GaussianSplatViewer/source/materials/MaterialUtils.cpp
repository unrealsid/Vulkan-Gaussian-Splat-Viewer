#include "materials/MaterialUtils.h"
#include "materials/Material.h"
#include "../../include/structs/scene/PushConstantBlock.h"
#include "vulkanapp/utils/DescriptorUtils.h"
#include "vulkanapp/utils/FileUtils.h"
#include "structs/EngineContext.h"

namespace material
{
    std::shared_ptr<Material> MaterialUtils::create_material(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path) const
    {
        //Setup push constants
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(PushConstantBlock);

        size_t shaderCodeSizes[2]{};
        char* shaderCodes[2]{};

        utils::FileUtils::load_shader(vertex_shader_path, shaderCodes[0], shaderCodeSizes[0]);
        utils::FileUtils::load_shader(fragment_shader_path, shaderCodes[1], shaderCodeSizes[1]);

        auto shader_object = std::make_unique<ShaderObject>();
        shader_object->create_shaders(engine_context.dispatch_table, shaderCodes[0], shaderCodeSizes[0], shaderCodes[1], shaderCodeSizes[1],
            nullptr, 0,
            &push_constant_range, 1);

        VkPipelineLayout pipeline_layout;

        //Create the pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = utils::DescriptorUtils::pipeline_layout_create_info(nullptr,  0, &push_constant_range, 1);
        engine_context.dispatch_table.createPipelineLayout(&pipelineLayoutInfo, VK_NULL_HANDLE, &pipeline_layout);

        //Create material
        auto material = make_shared<Material>(name, engine_context);
        material->add_shader_object(std::move(shader_object));
        material->add_pipeline_layout(pipeline_layout);

        return material;
    }
}
