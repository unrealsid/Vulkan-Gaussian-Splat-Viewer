#include "core/Engine.h"
#include <chrono>
#include "vulkanapp/VulkanCleanupQueue.h"
#include <iostream>

void core::Engine::create_window() const
{
    engine_context->window_manager = std::make_unique<platform::WindowManager>(*engine_context);
    engine_context->window_manager->create_window_sdl3("Vulkan Gaussian Splat");
}

void core::Engine::create_swapchain() const
{
    engine_context->swapchain_manager = std::make_unique<vulkanapp::SwapchainManager>(*engine_context);
    engine_context->swapchain_manager->create_swapchain();
}

void core::Engine::create_device() const
{
    engine_context->device_manager = std::make_unique<vulkanapp::DeviceManager>(*engine_context);
    engine_context->device_manager->device_init();
}

void core::Engine::create_cleanup() const
{
    vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context->swapchain_manager->cleanup()));
    vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context->device_manager->cleanup()));
    vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context->window_manager->destroy_window_sdl3()));
}

void core::Engine::init()
{
    engine_context = std::make_unique<EngineContext>();
    
    create_window();
    create_device();
    create_swapchain();
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
