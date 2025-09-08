#include "vulkanapp/SwapchainManager.h"

#include <iostream>
#include <vulkan/vulkan_core.h>

#include "structs/EngineContext.h"


vulkanapp::SwapchainManager::~SwapchainManager()
{
    
}

bool vulkanapp::SwapchainManager::create_swapchain()
{
    vkb::SwapchainBuilder swapchain_builder{ engine_context.device_manager->get_device() };
    
    auto swap_ret = swapchain_builder
        .set_old_swapchain(swapchain)
        .set_desired_format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) 
        .add_fallback_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
        .add_fallback_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
        .build();
        
    if (!swap_ret)
    {
        std::cout << "Failed to create swapchain: " << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
        return false;
    }

    // Clean up old swapchain if it exists
    if (swapchain.swapchain != VK_NULL_HANDLE)
    {
        cleanup_image_views();
        vkb::destroy_swapchain(swapchain);
    }
    
    swapchain = swap_ret.value();
    
    // Get swapchain images
    auto images = swapchain.get_images();
    if (!images)
    {
        std::cout << "Failed to get swapchain images: " << images.error().message() << "\n";
        return false;
    }
    swapchain_images = images.value();
    
    return true;
}

bool vulkanapp::SwapchainManager::recreate_swapchain()
{
    auto device_manager = engine_context.device_manager.get();
       
    
    // Wait for device to be idle
    engine_context.dispatch_table.deviceWaitIdle();

    // Recreate the swapchain
    if (!create_swapchain())
    {
        std::cout << "Failed to recreate swapchain\n";
        return false;
    }

    // Recreate dependent resources
    
    return true;
}

void vulkanapp::SwapchainManager::cleanup_image_views()
{
    for (auto& image_view : swapchain_image_views)
    {
        if (image_view != VK_NULL_HANDLE)
        {
            engine_context.dispatch_table.destroyImageView(image_view, nullptr);
        }
    }
    swapchain_image_views.clear();
}

void vulkanapp::SwapchainManager::cleanup_images()
{
    for(auto swapchain_image : swapchain_images)
    {
        engine_context.dispatch_table.destroyImage(swapchain_image, nullptr);
    }

    swapchain_images.clear();
}

void vulkanapp::SwapchainManager::cleanup()
{
    if (swapchain.swapchain != VK_NULL_HANDLE)
    {
        //cleanup_images();

        //cleanup_image_views();

        vkb::destroy_swapchain(swapchain);
        swapchain = {};
    }
    
}
