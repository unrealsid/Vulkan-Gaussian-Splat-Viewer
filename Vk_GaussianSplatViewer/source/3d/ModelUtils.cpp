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

    std::vector<glm::vec4> ModelUtils::load_gaussian_bounding_box()
    {
        std::vector<glm::vec4> cube = {
            // Back
            {-1.f, -1.f, -1.f, 1.f}, { 1.f, -1.f, -1.f, 1.f}, { 1.f,  1.f, -1.f, 1.f},
            { 1.f,  1.f, -1.f, 1.f}, {-1.f,  1.f, -1.f, 1.f}, {-1.f, -1.f, -1.f, 1.f},

            // Front
            {-1.f, -1.f,  1.f, 1.f}, { 1.f, -1.f,  1.f, 1.f}, { 1.f,  1.f,  1.f, 1.f},
            { 1.f,  1.f,  1.f, 1.f}, {-1.f,  1.f,  1.f, 1.f}, {-1.f, -1.f,  1.f, 1.f},

            // Bottom
            {-1.f, -1.f, -1.f, 1.f}, { 1.f, -1.f, -1.f, 1.f}, { 1.f, -1.f,  1.f, 1.f},
            { 1.f, -1.f,  1.f, 1.f}, {-1.f, -1.f,  1.f, 1.f}, {-1.f, -1.f, -1.f, 1.f},

            // Top
            {-1.f,  1.f, -1.f, 1.f}, { 1.f,  1.f, -1.f, 1.f}, { 1.f,  1.f,  1.f, 1.f},
            { 1.f,  1.f,  1.f, 1.f}, {-1.f,  1.f,  1.f, 1.f}, {-1.f,  1.f, -1.f, 1.f},

            // Left
            {-1.f, -1.f, -1.f, 1.f}, {-1.f,  1.f, -1.f, 1.f}, {-1.f,  1.f,  1.f, 1.f},
            {-1.f,  1.f,  1.f, 1.f}, {-1.f, -1.f,  1.f, 1.f}, {-1.f, -1.f, -1.f, 1.f},

            // Right
            { 1.f, -1.f, -1.f, 1.f}, { 1.f,  1.f, -1.f, 1.f}, { 1.f,  1.f,  1.f, 1.f},
            { 1.f,  1.f,  1.f, 1.f}, { 1.f, -1.f,  1.f, 1.f}, { 1.f, -1.f, -1.f, 1.f},
        };

        return cube;
    }

    splat_loader::GaussianSplatPlyLoader ModelUtils::load_placeholder_gaussian_model()
    {
        splat_loader::GaussianSplatPlyLoader ply;

        return ply;
    }

    splat_loader::GaussianSplatPlyLoader ModelUtils::load_gaussian_surfaces(const std::string& file_path)
    {
        splat_loader::GaussianSplatPlyLoader ply;

        if (!ply.load(file_path))
        {
            std::cerr << "Failed to load PLY\n";
            return {};
        }

        return ply;
    }
}
