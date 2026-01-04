#pragma once

#include "structs/GPU_Buffer.h"

struct GaussianGeometryData
{
    GPU_Buffer mesh_vertices_buffer;
    GPU_Buffer mesh_indices_buffer;

    GPU_Buffer gaussian_buffer;

    //How many surfaces has the uploader extracted?
    uint32_t gaussian_count = 0;
};
