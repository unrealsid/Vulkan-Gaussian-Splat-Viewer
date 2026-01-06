#pragma once

#include <vector>
#include <string>
#include <glm/vec4.hpp>

#include "structs/geometry/GaussianSurface.h"

namespace splat_loader
{
    class GaussianSplatPlyLoader
    {
    public:
        bool load(const std::string& file_path);

        [[nodiscard]] const std::vector<glm::vec4>& get_positions() const { return positions; }
        [[nodiscard]] const std::vector<glm::vec4>& get_scales() const { return scales; }
        [[nodiscard]] const std::vector<glm::vec4>& get_colors() const { return colors; }
        [[nodiscard]] const std::vector<glm::vec4>& get_quaternions() const { return quaternions; }
        [[nodiscard]] const std::vector<float>& get_alphas() const { return alphas; }

        [[nodiscard]] size_t get_count() const { return count; }

    private:
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> scales;
        std::vector<glm::vec4> colors;
        std::vector<glm::vec4> quaternions;
        std::vector<float> alphas;

        uint32_t count = 0;
    };
}
