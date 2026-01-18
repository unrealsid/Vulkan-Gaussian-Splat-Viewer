#pragma once
#include <cassert>
#include <unordered_map>
#include <vector>

#include "config/Config.inl"
#include "renderer/GPU_BufferContainer.h"
#include "structs/EngineContext.h"
#include "structs/Renderable.h"
#include "vulkanapp/utils/MemoryUtils.h"

struct EngineContext;

namespace entity_3d
{
    class Scene
    {
    public:
        explicit Scene(EngineContext& engine_context) : last_index(0), engine_context(engine_context){ }

        //Adds a new renderable object to the scene given a vertex or index buffer
        template<typename V>
        void add_new_renderable(const std::string& object_name,
                                const std::vector<V>& vertex_buffer,
                                const std::vector<uint32_t>& index_buffer = {})
        {
            //make sure we are under the object count
            assert(last_index < max_object_count);

            Renderable renderable;
            core::rendering::GPU_BufferContainer gpu_buffer_container(engine_context);

            //Allocate vertex buffer
            gpu_buffer_container.allocate_named_buffer(object_name, vertex_buffer, BufferAllocationType::VertexAllocationWithStaging);

            if (const auto buffer = gpu_buffer_container.get_buffer(object_name))
            {
                renderable.vertex_buffer = *buffer;
            }

            renderable.object_index = ++last_index;

            //add an entry for this object
            if (auto model_matrix_buffer = gpu_buffer_container.get_buffer("model_matrix_buffer"))
            {
                auto model_matrix = glm::vec4(1.0f);

                //The object id is the offset in the matrix array
                auto offset = renderable.object_index;
                utils::MemoryUtils::map_persistent_data(engine_context.device_manager->get_allocator(),
                                                        model_matrix_buffer->allocation,
                                                        model_matrix_buffer->allocation_info, &model_matrix, sizeof(glm::mat4), offset);
            }

            renderables.push_back(renderable);
        }

    private:
        //Last object index used
        uint32_t last_index;

        //Objects to render on screen
        std::vector<Renderable> renderables;

        //Stores names to object id
        std::unordered_map<std::string, uint32_t> name_to_id;

        EngineContext& engine_context;
    };
}
