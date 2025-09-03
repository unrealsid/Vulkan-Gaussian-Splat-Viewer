set(
    Header_Files
	"include/Vk_GaussianSplatViewer.hpp"
)
source_group(
	TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" 
	PREFIX "Header Files" 
	FILES ${Header_Files}
)

set(
    Source_Files
	"source/main.cpp"
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

set(ENGINE_PROJECT_NAME "Vk_GaussianSplatViewer")

add_executable(
    ${ENGINE_PROJECT_NAME} ${ALL_FILES}
) 

target_include_directories(
    ${ENGINE_PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE )

# ------------------------------------------------------
# 2. Add Vulkan and related libraries (vma/glm etc) from Vulkan SDK 
#  I'm assuming we're using GLM, vma etc from the SDK installer
# ------------------------------------------------------
if (NOT "${VULKAN_SDK_PATH}" STREQUAL "")
    message(STATUS "Using Vulkan SDK from: ${VULKAN_SDK_PATH}")

    # Add include directories
    target_include_directories(Vk_GaussianSplatViewer PUBLIC
        "${VULKAN_SDK_PATH}/Include" # Main Vulkan headers
        "${VULKAN_SDK_PATH}/Include/vma" 
        "${VULKAN_SDK_PATH}/Include/glm"
        # If vk_hpp is in a specific subdir like Include/vk_hpp, add it here:
        "${VULKAN_SDK_PATH}/Include/vulkan"
    )

    # Add library directories and link libraries
    # Assuming Windows library structure with .lib files
    set(VULKAN_SDK_LIB_DIR "${VULKAN_SDK_PATH}/lib")

    target_link_libraries(Vk_GaussianSplatViewer
        # Vulkan main library (for Vulkan-1.dll)
        "${VULKAN_SDK_LIB_DIR}/vulkan-1.lib"
    )

else()
    message(FATAL_ERROR "VULKAN_SDK_PATH is not set. Please set it in the CMake configuration to point to your Vulkan SDK installation.")
endif()