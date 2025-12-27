#include "materials/Material.h"
#include "materials/ShaderObject.h"
#include "structs//RenderContext.h"

#include "structs//PushConstantBlock.h"
#include "vulkanapp/utils/DescriptorUtils.h"
#include "vulkanapp/utils/FileUtils.h"

namespace material
{
    Material::Material(std::string material_name, RenderContext* render_context) :
        material_name(std::move(material_name)), descriptor_set(nullptr), pipeline_layout(nullptr),
        render_context(render_context)
    {
        
    }

    void Material::init()
    {
    }

    void Material::add_shader_object(std::unique_ptr<ShaderObject> shader_object)
    {
        this->shader_object = std::move(shader_object);
    }

    void Material::add_pipeline_layout(VkPipelineLayout pipeline_layout)
    {
        this->pipeline_layout = pipeline_layout;
    }

    void Material::add_descriptor_set(VkDescriptorSet descriptor_set)
    {
        this->descriptor_set = descriptor_set;
    }
}
