#include "3d/ModelUtils.h"

#include <iostream>
#include <vector>
#include "../../include/structs/geometry/Vertex2D.h"
#include "3d/GaussianSplatPlyLoader.h"

namespace entity_3d
{
    std::vector<Vertex2D> ModelUtils::load_triangle_model()
    {
        return std::vector<Vertex2D>
        {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
    }

    std::vector<GaussianSurface> ModelUtils::load_gaussian_surfaces(const std::string& file_path)
    {
        splat_loader::GaussianSplatPlyLoader ply;

        if (!ply.load(file_path))
        {
            std::cerr << "Failed to load PLY\n";
            return {};
        }

        return ply.get_gaussians();
    }
}
