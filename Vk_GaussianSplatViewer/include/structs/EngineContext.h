#pragma once

#include <memory>
#include <VkBootstrapDispatch.h>

#include "GPU_Buffer.h"
#include "vulkanapp/SwapchainManager.h"
#include "platform/WindowManager.h"
#include "renderer/Renderer.h"
#include "vulkanapp/DeviceManager.h"

struct EngineContext
{
    std::unique_ptr<platform::WindowManager> window_manager;
    std::unique_ptr<vulkanapp::DeviceManager> device_manager;
    std::unique_ptr<vulkanapp::SwapchainManager> swapchain_manager;
    std::unique_ptr<core::renderer::Renderer> renderer;

    //TODO: Remove from here later. Needs proper resource allocation and storage. Only for testing
    GPU_Buffer mesh_vertices_buffer;
    GPU_Buffer mesh_indices_buffer;

    //TODO: Remove later
    bool start_rendering = false;

    vkb::InstanceDispatchTable instance_dispatch_table;
    vkb::DispatchTable dispatch_table;
};
