#include "GaussianSplatPly.h"

#include <fstream>
#include <iostream>
#include <memory>
#include "tinyply.h"

using namespace tinyply;

namespace splat_loader
{
    namespace
    {
        template<typename T>
        void copy_property(
            const std::shared_ptr<PlyData>& data,
            T* dst,
            size_t count)
        {
            std::memcpy(dst, data->buffer.get(), sizeof(T) * count);
        }
    }

    bool GaussianSplatPly::load(const std::string& file_path)
    {
        std::ifstream file(file_path, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open PLY file: " << file_path << std::endl;
            return false;
        }

        PlyFile ply_file;
        ply_file.parse_header(file);

        std::shared_ptr<PlyData> x, y, z;
        std::shared_ptr<PlyData> nx, ny, nz;
        std::shared_ptr<PlyData> opacity;
        std::shared_ptr<PlyData> scale_0, scale_1, scale_2;
        std::shared_ptr<PlyData> rot_0, rot_1, rot_2, rot_3;

        std::shared_ptr<PlyData> f_dc[3];
        std::shared_ptr<PlyData> f_rest[45];

        x  = ply_file.request_properties_from_element("vertex", { "x" });
        y  = ply_file.request_properties_from_element("vertex", { "y" });
        z  = ply_file.request_properties_from_element("vertex", { "z" });

        nx = ply_file.request_properties_from_element("vertex", { "nx" });
        ny = ply_file.request_properties_from_element("vertex", { "ny" });
        nz = ply_file.request_properties_from_element("vertex", { "nz" });

        for (int i = 0; i < 3; ++i)
        {
            f_dc[i] = ply_file.request_properties_from_element(
                "vertex", { "f_dc_" + std::to_string(i) });
        }

        for (int i = 0; i < 45; ++i)
        {
            f_rest[i] = ply_file.request_properties_from_element(
                "vertex", { "f_rest_" + std::to_string(i) });
        }

        opacity = ply_file.request_properties_from_element("vertex", { "opacity" });

        scale_0 = ply_file.request_properties_from_element("vertex", { "scale_0" });
        scale_1 = ply_file.request_properties_from_element("vertex", { "scale_1" });
        scale_2 = ply_file.request_properties_from_element("vertex", { "scale_2" });

        rot_0 = ply_file.request_properties_from_element("vertex", { "rot_0" });
        rot_1 = ply_file.request_properties_from_element("vertex", { "rot_1" });
        rot_2 = ply_file.request_properties_from_element("vertex", { "rot_2" });
        rot_3 = ply_file.request_properties_from_element("vertex", { "rot_3" });

        ply_file.read(file);

        const size_t vertex_count = x->count;
        gaussians.resize(vertex_count);

        std::vector<float> temp(vertex_count);

        copy_property(x, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i) gaussians[i].x = temp[i];

        copy_property(y, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i) gaussians[i].y = temp[i];

        copy_property(z, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i) gaussians[i].z = temp[i];

        copy_property(nx, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i) gaussians[i].nx = temp[i];

        copy_property(ny, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i) gaussians[i].ny = temp[i];

        copy_property(nz, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i) gaussians[i].nz = temp[i];

        for (int c = 0; c < 3; ++c)
        {
            copy_property(f_dc[c], temp.data(), vertex_count);
            for (size_t i = 0; i < vertex_count; ++i)
                gaussians[i].f_dc[c] = temp[i];
        }

        for (int c = 0; c < 45; ++c)
        {
            copy_property(f_rest[c], temp.data(), vertex_count);
            for (size_t i = 0; i < vertex_count; ++i)
                gaussians[i].f_rest[c] = temp[i];
        }

        copy_property(opacity, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].opacity = temp[i];

        copy_property(scale_0, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].scale[0] = temp[i];

        copy_property(scale_1, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].scale[1] = temp[i];

        copy_property(scale_2, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].scale[2] = temp[i];

        copy_property(rot_0, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].rotation[0] = temp[i];

        copy_property(rot_1, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].rotation[1] = temp[i];

        copy_property(rot_2, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].rotation[2] = temp[i];

        copy_property(rot_3, temp.data(), vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
            gaussians[i].rotation[3] = temp[i];

        return true;
    }
}
