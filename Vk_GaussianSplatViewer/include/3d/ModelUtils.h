#pragma once
#include <string>

#include "GaussianSplatPlyLoader.h"
#include "structs/geometry/Vertex2D.h"

namespace entity_3d
{
    class ModelUtils
    {
        public:
        static std::vector<Vertex2D> load_triangle_model();

        static std::vector<glm::vec4> load_gaussian_bounding_box();

        static std::vector<glm::vec4> load_tetrahedron();

        static splat_loader::GaussianSplatPlyLoader load_placeholder_gaussian_model();

        static splat_loader::GaussianSplatPlyLoader load_gaussian_surfaces(const std::string& file_path);
    };
}
