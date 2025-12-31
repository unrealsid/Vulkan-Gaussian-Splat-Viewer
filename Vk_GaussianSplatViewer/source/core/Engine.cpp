#include "core/Engine.h"
#include <chrono>

#include "3d/ModelUtils.h"
#include "vulkanapp/VulkanCleanupQueue.h"
#include "config/Config.inl"
#include "renderer/Renderer.h"
#include "renderer/RenderPass.h"
#include "structs/WindowCreateParams.h"

#include <imgui_impl_sdl3.h>

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

void core::Engine::gaussian_surface_init(const std::vector<GaussianSurface>& gaussian_surfaces) const
{
    engine_context->renderer->allocate_gaussian_buffer(gaussian_surfaces);
    engine_context->gaussian_count = gaussian_surfaces.size();
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

void core::Engine::handle_input(bool& is_running, camera::FirstPersonCamera* camera, double delta_time)
{
    SDL_Event event;

    const ImGuiIO& io = ImGui::GetIO();

    // Process events from the OS
    while (SDL_PollEvent(&event))
    {
        // 1. Always pass events to ImGui
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT)
        {
            is_running = false;
            continue;
        }

        const bool imgui_wants_mouse    = io.WantCaptureMouse;
        const bool imgui_wants_keyboard = io.WantCaptureKeyboard;

        if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            if (!imgui_wants_mouse &&
                (event.motion.state & SDL_BUTTON_RMASK))
            {
                camera->process_mouse_movement(
                    event.motion.xrel,
                    event.motion.yrel
                );
            }
        }

        // 3. Mouse wheel → zoom only if ImGui does NOT want mouse
        if (event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            if (!imgui_wants_mouse)
            {
                camera->process_mouse_scroll(event.wheel.y);
            }
        }

        // Optional: Handle mouse wheel for FOV zoom
        if (event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            if (!io.WantCaptureMouse)
            {
                camera->process_mouse_scroll(event.wheel.y);
            }
        }
    }

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);
    if (!io.WantCaptureKeyboard)
    {
        camera->process_keyboard(keyboard_state, static_cast<float>(delta_time));
    }
}

void core::Engine::run() const
{
    bool is_running = true;
   
    auto previous_time = std::chrono::high_resolution_clock::now();
    auto camera = engine_context->renderer->get_camera();

    while (is_running)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = current_time - previous_time;
        double delta_time = elapsed.count();

        handle_input(is_running, camera, delta_time);

        // update();

        // draw();
        engine_context->renderer->renderer_update();

        previous_time = current_time;
    }
}

void core::Engine::cleanup()
{
    vulkanapp::VulkanCleanupQueue::flush();
}
