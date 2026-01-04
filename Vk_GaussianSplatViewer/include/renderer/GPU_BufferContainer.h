#pragma once

#include <vector>

#include "structs/GPU_Buffer.h"
#include "structs/geometry/GaussianSurface.h"

struct EngineContext;

namespace camera
{
    class FirstPersonCamera;
}

namespace core::renderer
{
    class GPU_BufferContainer
    {
    public:
        GPU_BufferContainer(EngineContext& engine_context);

        //Camera Buffer
        GPU_Buffer camera_data_buffer;

        //Gaussian Buffers
        GPU_Buffer mesh_vertices_buffer;
        GPU_Buffer mesh_indices_buffer;

        GPU_Buffer gaussian_buffer;

        //How many surfaces has the uploader extracted?
        uint32_t gaussian_count = 0;

        void allocate_camera_buffer(const camera::FirstPersonCamera& first_person_camera);

        void allocate_gaussian_surface_buffer(const std::vector<GaussianSurface>& gaussians);

    private:
        EngineContext& engine_context;
    };
}
