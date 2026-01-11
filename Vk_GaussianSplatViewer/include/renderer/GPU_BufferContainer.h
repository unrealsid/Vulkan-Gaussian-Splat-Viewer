#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "structs/GPU_Buffer.h"

struct EngineContext;

namespace camera
{
    class FirstPersonCamera;
}

namespace core::rendering
{
    class GPU_BufferContainer
    {
    public:
        GPU_BufferContainer(EngineContext& engine_context);

        //Camera Buffer
        GPU_Buffer camera_data_buffer;

        //How many surfaces has the uploader extracted?
        uint32_t gaussian_count = 0;

        void allocate_camera_buffer(const camera::FirstPersonCamera& first_person_camera);

        template<typename  N>
        void allocate_gaussian_buffer(const std::string& buffer_name, const std::vector<N>& gaussian_parameters);

        void set_buffer(const std::string& buffer_name, const GPU_Buffer& buffer);

        GPU_Buffer* get_buffer(const std::string& buffer_name);

        void destroy_buffers();

    private:
        EngineContext& engine_context;

        std::unordered_map<std::string, GPU_Buffer> gaussian_buffers;
    };
}
