set(
    Header_Files
	"include/Vk_GaussianSplatViewer.hpp"
	"include/DeviceManager.h"
)
source_group(
	TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" 
	PREFIX "Header Files" 
	FILES ${Header_Files}
)

set(
    Source_Files
	"source/main.cpp"
	"source/DeviceManager.cpp"
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

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE Vulkan::Vulkan vk-bootstrap::vk-bootstrap)