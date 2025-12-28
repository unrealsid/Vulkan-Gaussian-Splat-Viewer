#pragma once

#include <memory>
#include "structs/EngineContext.h"

//Core engine class
namespace core
{
    class Engine
    {
    public:
        void init();
        void run() const;
        void cleanup();

    private:
        //Stored here pass this as a pointer or reference wherever needed
        std::unique_ptr<EngineContext> engine_context;

        void create_window() const;
        void create_renderer() const;

        //TODO: Change this later:
        void geometry_init() const;

        //Orders and stores cleanup function for resource clear
        void create_cleanup() const;
    };
}
