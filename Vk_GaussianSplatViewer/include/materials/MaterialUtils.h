#pragma once
#include "structs/engine/RenderContext.h"
#include "Material.h"
#include <string>

namespace material
{
    class MaterialUtils
    {
    public:

        MaterialUtils(RenderContext* render_context) : render_context(render_context)
        {
            vertex_shader_path = R"(C:\Users\Sid\Documents\Visual Studio 2022\Code\VkLiteRenderer\VkLiteRenderer\shaders\triangle.vert.spv)";
            fragment_shader_path = R"(C:\Users\Sid\Documents\Visual Studio 2022\Code\VkLiteRenderer\VkLiteRenderer\shaders\triangle.frag.spv)";
        }

        std::shared_ptr<Material> create_material(const std::string& name) const;

    private:
        RenderContext* render_context;
        std::string vertex_shader_path;
        std::string fragment_shader_path;

    };
}
