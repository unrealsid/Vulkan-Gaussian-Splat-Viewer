#include "renderer/subpasses/ImGuiPass.h"
#include "structs/EngineContext.h"
#include "vulkanapp/DeviceManager.h"
#include "vulkanapp/SwapchainManager.h"
#include <iostream>


namespace core::rendering
{
    ImGuiPass::ImGuiPass(EngineContext& engine_context, uint32_t max_frames_in_flight)
        : Subpass(engine_context, max_frames_in_flight)
    {
        init_imgui();
    }

    ImGuiPass::~ImGuiPass()
    {
        ImGuiPass::cleanup();
    }

    void ImGuiPass::init_imgui()
    {
        // Create Descriptor Pool for ImGui
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
        pool_info.pPoolSizes = pool_sizes;

        if (engine_context.dispatch_table.createDescriptorPool(&pool_info, nullptr, &imgui_pool) != VK_SUCCESS)
        {
            std::cerr << "Failed to create ImGui descriptor pool" << std::endl;
            return;
        }

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForVulkan(engine_context.window_manager->get_window());
        ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_4, [](const char* function_name, void* user_data)
        {
            auto instance = static_cast<VkInstance>(user_data);
            return vkGetInstanceProcAddr(instance, function_name);
        }, engine_context.instance_dispatch_table.instance);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device_manager->get_instance().instance;
        init_info.PhysicalDevice = device_manager->get_physical_device().physical_device;
        init_info.Device = device_manager->get_device().device;
        init_info.QueueFamily = device_manager->get_device().get_queue_index(vkb::QueueType::graphics).value();
        init_info.Queue = device_manager->get_graphics_queue();
        init_info.DescriptorPool = imgui_pool;
        init_info.MinImageCount = max_frames_in_flight;
        init_info.ImageCount = swapchain_manager->get_image_count();
        init_info.UseDynamicRendering = true;
        init_info.UseShaderObjects = true;

        ImGui_ImplVulkan_Init(&init_info);
    }

#pragma optimize("", off)

    void ImGuiPass::record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, bool is_last)
    {
        setup_color_attachment(image_index, { {0.0f, 0.0f, 0.0f, 1.0f} });
        color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // Don't clear what GeometryPass did
        
        depth_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depth_attachment_info.imageView = VK_NULL_HANDLE;

        begin_rendering();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        // ImGui::Text("Gaussian Splat Loader");
        // {
        //     bool show_demo_window = true;
        //     bool show_another_window = false;
        //     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        //     static float f = 0.0f;
        //     static int counter = 0;
        //
        //     ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        //
        //     ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //     ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //     ImGui::Checkbox("Another Window", &show_another_window);
        //
        //     ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //     ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        //
        //     if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //         counter++;
        //     ImGui::SameLine();
        //     ImGui::Text("counter = %d", counter);
        //     ImGui::End();
        // }


        ImGui::Begin("Gaussian Splat Loader");

        ImGui::Text("Load Gaussian Splat/PointCloud");

        static char text_buffer[256] = "";
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##filepath", text_buffer, IM_ARRAYSIZE(text_buffer));
        ImGui::PopItemWidth();

        if (ImGui::Button("Load File", ImVec2(-1, 0)))
        {
            engine_context.ui_action_manager->queue_string_action(UIAction::ALLOCATE_SPLAT_MEMORY, text_buffer);
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *command_buffer);

        end_rendering();
        end_command_buffer_recording(image_index, is_last);
    }


#pragma optimize("", on)

    void ImGuiPass::cleanup()
    {
        if (imgui_pool != VK_NULL_HANDLE)
        {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();

            engine_context.dispatch_table.destroyDescriptorPool(imgui_pool, nullptr);
            imgui_pool = VK_NULL_HANDLE;
        }
    }
}
