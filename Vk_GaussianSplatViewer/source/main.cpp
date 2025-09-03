// Vk_GaussianSplatViewer.cpp : Defines the entry point for the application.
//

#include "Vk_GaussianSplatViewer.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;

int main()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    // Enumerate physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) 
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    std::cout << "Found " << deviceCount << " Vulkan-capable device(s)." << std::endl;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 rotated = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 0, 1));
    glm::vec4 vec(1, 0, 0, 1);
    glm::vec4 result = rotated * vec;

    std::cout << "Rotated vector = ("
        << result.x << ", " << result.y << ", "
        << result.z << ", " << result.w << ")" << std::endl;

    vkDestroyInstance(instance, nullptr);
    return 0;
}
