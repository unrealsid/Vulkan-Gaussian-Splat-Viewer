#pragma once

#include <memory>

#include "vulkanapp/SwapchainManager.h"
#include "platform/WindowManager.h"
#include "structs/GPU_Buffer.h"
#include "vulkanapp/DeviceManager.h"
#include "renderer/RenderPass.h"

/*
 * Stores Rendering content. Only used on the Rendering Thread
 */

struct RenderContext
{
    std::unique_ptr<vulkanapp::DeviceManager> device_manager;
    std::unique_ptr<vulkanapp::SwapchainManager> swapchain_manager;
    std::unique_ptr<core::renderer::RenderPass> render_pass;

    //TODO: Remove from here later. Needs proper resource allocation and storage. Only for testing
    GPU_Buffer mesh_vertices_buffer;
    GPU_Buffer mesh_indices_buffer;

    vkb::InstanceDispatchTable instance_dispatch_table;
    vkb::DispatchTable dispatch_table;
};
