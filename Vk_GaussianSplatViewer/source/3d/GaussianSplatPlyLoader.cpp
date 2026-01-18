#include "3d/GaussianSplatPlyLoader.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <tinyply.h>

using namespace tinyply;

namespace splat_loader
{
    namespace
    {
        template<typename T>
        inline void copy_property(
            const std::shared_ptr<PlyData>& data,
            T* dst,
            size_t count)
        {
            std::memcpy(dst, data->buffer.get(), sizeof(T) * count);
        }
    }

    bool GaussianSplatPlyLoader::load(const std::string& file_path)
    {
        std::ifstream file(file_path, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open PLY file: " << file_path << std::endl;
            return false;
        }

        PlyFile ply_file;
        ply_file.parse_header(file);

        // --- Request properties ---
        auto x  = ply_file.request_properties_from_element("vertex", { "x" });
        auto y  = ply_file.request_properties_from_element("vertex", { "y" });
        auto z  = ply_file.request_properties_from_element("vertex", { "z" });

        auto nx = ply_file.request_properties_from_element("vertex", { "nx" });
        auto ny = ply_file.request_properties_from_element("vertex", { "ny" });
        auto nz = ply_file.request_properties_from_element("vertex", { "nz" });

        auto opacity = ply_file.request_properties_from_element("vertex", { "opacity" });

        auto scale_0 = ply_file.request_properties_from_element("vertex", { "scale_0" });
        auto scale_1 = ply_file.request_properties_from_element("vertex", { "scale_1" });
        auto scale_2 = ply_file.request_properties_from_element("vertex", { "scale_2" });

        auto rot_0 = ply_file.request_properties_from_element("vertex", { "rot_0" });
        auto rot_1 = ply_file.request_properties_from_element("vertex", { "rot_1" });
        auto rot_2 = ply_file.request_properties_from_element("vertex", { "rot_2" });
        auto rot_3 = ply_file.request_properties_from_element("vertex", { "rot_3" });

        std::shared_ptr<PlyData> f_dc[3];
        std::shared_ptr<PlyData> f_rest[45];

        for (int i = 0; i < 3; ++i)
            f_dc[i] = ply_file.request_properties_from_element(
                "vertex", { "f_dc_" + std::to_string(i) });

        for (int i = 0; i < 45; ++i)
            f_rest[i] = ply_file.request_properties_from_element(
                "vertex", { "f_rest_" + std::to_string(i) });

        // Read data
        ply_file.read(file);

        const size_t vertex_count = x->count;
        gaussians.resize(vertex_count);

        // --- Allocate SoA buffers ---
        std::vector<float> xs(vertex_count), ys(vertex_count), zs(vertex_count);
        std::vector<float> nxs(vertex_count), nys(vertex_count), nzs(vertex_count);
        std::vector<float> opacity_arr(vertex_count);

        std::vector<float> scale[3];
        std::vector<float> rotation[4];
        std::vector<float> f_dc_arr[3];
        std::vector<float> f_rest_arr[45];

        for (int i = 0; i < 3; ++i)
        {
            scale[i].resize(vertex_count);
            f_dc_arr[i].resize(vertex_count);
        }

        for (auto & i : rotation)
        {
            i.resize(vertex_count);
        }

        for (auto & i : f_rest_arr)
        {
            i.resize(vertex_count);
        }

        copy_property(x,  xs.data(), vertex_count);
        copy_property(y,  ys.data(), vertex_count);
        copy_property(z,  zs.data(), vertex_count);

        copy_property(nx, nxs.data(), vertex_count);
        copy_property(ny, nys.data(), vertex_count);
        copy_property(nz, nzs.data(), vertex_count);

        copy_property(opacity, opacity_arr.data(), vertex_count);

        copy_property(scale_0, scale[0].data(), vertex_count);
        copy_property(scale_1, scale[1].data(), vertex_count);
        copy_property(scale_2, scale[2].data(), vertex_count);

        copy_property(rot_0, rotation[0].data(), vertex_count);
        copy_property(rot_1, rotation[1].data(), vertex_count);
        copy_property(rot_2, rotation[2].data(), vertex_count);
        copy_property(rot_3, rotation[3].data(), vertex_count);

        for (int i = 0; i < 3; ++i)
            copy_property(f_dc[i], f_dc_arr[i].data(), vertex_count);

        for (int i = 0; i < 45; ++i)
            copy_property(f_rest[i], f_rest_arr[i].data(), vertex_count);

        GaussianSurface* out = gaussians.data();

        for (size_t i = 0; i < vertex_count; ++i)
        {
            auto& g = out[i];

            g.position[0] = xs[i];
            g.position[1] = ys[i];
            g.position[2] = zs[i];

            g.normal[0] = nxs[i];
            g.normal[1] = nys[i];
            g.normal[2] = nzs[i];

            for (int c = 0; c < 3; ++c)
                g.f_dc[c] = f_dc_arr[c][i];

            for (int c = 0; c < 45; ++c)
                g.f_rest[c] = f_rest_arr[c][i];

            g.opacity = opacity_arr[i];

            for (int c = 0; c < 3; ++c)
                g.scale[c] = scale[c][i];

            for (int c = 0; c < 4; ++c)
                g.rotation[c] = rotation[c][i];
        }

        return true;
    }
}