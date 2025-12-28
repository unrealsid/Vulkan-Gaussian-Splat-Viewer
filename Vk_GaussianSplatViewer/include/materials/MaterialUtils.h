#pragma once

#include "Material.h"
#include <string>

struct EngineContext;

namespace material
{
    class MaterialUtils
    {
    public:

        MaterialUtils(EngineContext& engine_context) : engine_context(engine_context)
        {
            vertex_shader_path = R"(C:\Users\Sid\Documents\Visual Studio 2022\Code\VkLiteRenderer\VkLiteRenderer\shaders\triangle.vert.spv)";
            fragment_shader_path = R"(C:\Users\Sid\Documents\Visual Studio 2022\Code\VkLiteRenderer\VkLiteRenderer\shaders\triangle.frag.spv)";
        }

        [[nodiscard]] std::shared_ptr<Material> create_material(const std::string& name) const;

    private:
        EngineContext& engine_context;
        std::string vertex_shader_path;
        std::string fragment_shader_path;

    };
}
