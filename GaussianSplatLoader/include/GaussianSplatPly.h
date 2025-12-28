#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "structs/Gaussian.h"

namespace splat_loader
{
    class GaussianSplatPly
    {
    public:
        bool load(const std::string& file_path);

        [[nodiscard]] const std::vector<Gaussian>& get_gaussians() const { return gaussians; }

    private:
        std::vector<Gaussian> gaussians;
    };
} // splat_loader
