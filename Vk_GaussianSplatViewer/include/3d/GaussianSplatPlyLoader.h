#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "../structs/geometry/GaussianSurface.h"

namespace splat_loader
{
    class GaussianSplatPlyLoader
    {
    public:
        bool load(const std::string& file_path);

        [[nodiscard]] const std::vector<GaussianSurface>& get_gaussians() const { return gaussians; }

    private:
        std::vector<GaussianSurface> gaussians;
    };
} // splat_loader
