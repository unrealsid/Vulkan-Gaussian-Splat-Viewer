#include "3d/ModelUtils.h"

#include <vector>
#include "../../include/structs/geometry/Vertex2D.h"

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
}
