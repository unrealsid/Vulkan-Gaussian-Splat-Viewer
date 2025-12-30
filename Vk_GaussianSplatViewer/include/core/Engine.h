#pragma once

#include <memory>
#include "structs/EngineContext.h"

struct GaussianSurface;

//Core engine class
namespace core
{
    class Engine
    {
    public:
        void init();

        //TODO: Change this later:
        void gaussian_surface_init(const std::vector<GaussianSurface>& gaussian_surfaces) const;

        void run() const;
        void cleanup();

    private:
        //Stored here pass this as a pointer or reference wherever needed
        std::unique_ptr<EngineContext> engine_context;

        void create_window() const;
        void create_renderer() const;

        //Orders and stores cleanup function for resource clear
        void create_cleanup() const;
    };
}
