#pragma once
#include "structs/Vertex2D.h"

namespace entity_3d
{
    class ModelUtils
    {
        public:
            static std::vector<Vertex2D> load_triangle_model();
    };
}
