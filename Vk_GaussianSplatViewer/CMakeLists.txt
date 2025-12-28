set(
    Header_Files
	"include/vulkanapp/feature_activator/VulkanFeatureActivator.h"

	"include/structs/EngineContext.h"
	"include/structs/GPU_Buffer.h"
	"include/structs/LoadedImageData.h"
	"include/structs/PushConstantBlock.h"
	"include/structs/Vertex.h"
	"include/structs/Vk_Image.h"
	
	"include/platform/WindowManager.h"
	"include/core/Engine.h"
	"include/config/Config.inl"
	"include/renderer/Subpass.h"
	"include/3d/ModelUtils.h"
	"include/enums/PresentationImageType.h"
	"include/materials/Material.h"
	"include/materials/MaterialUtils.h"
	"include/materials/ShaderObject.h"
	"include/renderer/subpasses/GeometryPass.h"
	"include/renderer/Renderer.h"
	"include/renderer/RenderPass.h"
	"include/structs/WindowCreateParams.h"

)
source_group(
	TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" 
	PREFIX "Header Files" 
	FILES ${Header_Files}
)

set(
    Source_Files
	"source/main.cpp"
	"source/vulkanapp/DeviceManager.cpp"
	"source/vulkanapp/SwapchainManager.cpp"
	"source/vulkanapp/VulkanCleanupQueue.cpp"
	"source/vulkanapp/feature_activator/VulkanFeatureActivator.cpp"
	"source/platform/WindowManager.cpp"
	"source/core/Engine.cpp"
	
	"source/vulkanapp/utils/DescriptorUtils.cpp"
	"source/vulkanapp/utils/FileUtils.cpp"
	"source/vulkanapp/utils/ImageUtils.cpp"
	"source/vulkanapp/utils/MemoryUtils.cpp"
	"source/vulkanapp/utils/RenderUtils.cpp"
	"source/vulkanapp/utils/Vk_Utils.cpp"
	"source/3d/ModelUtils.cpp"
	"source/materials/ShaderObject.cpp"
	"source/materials/MaterialUtils.cpp"
	"source/materials/Material.cpp"
	"source/render/subpasses/GeometryPass.cpp"
	"source/render/Renderer.cpp"
	"source/render/Subpass.cpp"
	"source/render/RenderPass.cpp"
	"source/render/RendererVulkan.cpp"
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/source" 
    PREFIX "Source Files" 
    FILES ${Source_Files}
)

set(
    ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

find_package(Vulkan REQUIRED)

set(ENGINE_PROJECT_NAME "Vk_GaussianSplatViewer")

add_executable(
    ${ENGINE_PROJECT_NAME} ${ALL_FILES}
) 

target_include_directories(
    ${ENGINE_PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(NOT TARGET tinyply)
	message(SEND_ERROR "Attempt to link to non-existent tinyply.")
endif()

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE Vulkan::Vulkan 
													vk-bootstrap::vk-bootstrap 
													SDL3::SDL3
													STB-image::STB-image
													tinyply)