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
            vertex_shader_path = R"(D:\Projects\CPP\Vk_GaussianSplat\Vk_GaussianSplatViewer\shaders\gaussian_surface\gaussian.vert.spv)";
            fragment_shader_path = R"(D:\Projects\CPP\Vk_GaussianSplat\Vk_GaussianSplatViewer\shaders\gaussian_surface\gaussian.frag.spv)";
        }

        [[nodiscard]] std::shared_ptr<Material> create_material(const std::string& name) const;

    private:
        EngineContext& engine_context;
        std::string vertex_shader_path;
        std::string fragment_shader_path;

    };
}
