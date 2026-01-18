#pragma once

#include "structs/GPU_Buffer.h"

//Defines an object that can be rendered
struct Renderable
{
    uint32_t object_index = 0;

    GPU_Buffer vertex_buffer;
    GPU_Buffer index_buffer;
};
