#pragma once
#include <string>
#include <vector>

#include "structs/geometry/Vertex2D.h"
#include "structs/geometry/GaussianSurface.h"

namespace entity_3d
{
    class ModelUtils
    {
        public:
        static std::vector<Vertex2D> load_triangle_model();

        static std::vector<GaussianSurface> load_placeholder_gaussian_model();

        static std::vector<GaussianSurface> load_gaussian_surfaces(const std::string& file_path);
    };
}
