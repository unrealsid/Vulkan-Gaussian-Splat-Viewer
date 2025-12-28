#include "core/Engine.h"
#include <chrono>
#include "vulkanapp/VulkanCleanupQueue.h"
#include "config/Config.inl"
#include "renderer/Renderer.h"
#include "structs/WindowCreateParams.h"

void core::Engine::create_window() const
{
    engine_context->window_manager = std::make_unique<platform::WindowManager>(*engine_context);
    WindowCreateParams window_create_params = { window_width, window_height, window_title };
    engine_context->window_manager->create_window_sdl3(window_create_params, true);
}

void core::Engine::create_renderer() const
{
    engine_context->renderer = std::make_unique<renderer::Renderer>(*engine_context);
    engine_context->renderer->renderer_init();
}

void core::Engine::create_cleanup() const
{
    engine_context->renderer->init_cleanup();
    vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context->window_manager->destroy_window_sdl3()));
}

void core::Engine::init()
{
    engine_context = std::make_unique<EngineContext>();
    
    create_window();
    create_renderer();
    create_cleanup();
}

void core::Engine::run() const
{
    bool is_running = true;
    SDL_Event event;

    auto window_manager = engine_context->window_manager.get();
    auto previous_time = std::chrono::high_resolution_clock::now();

    while (is_running)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = current_time - previous_time;
        double delta_time = elapsed.count();

        window_manager->update_mouse_position();
        window_manager->get_local_mouse_xy();
        
        // Process events from the OS
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running = false;
            }
        }

        // update();

        // draw();

        previous_time = current_time;
    }
}

void core::Engine::cleanup()
{
    vulkanapp::VulkanCleanupQueue::flush();
}
