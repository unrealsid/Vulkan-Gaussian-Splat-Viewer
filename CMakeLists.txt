cmake_minimum_required(
	VERSION 4.0.0 
	FATAL_ERROR
)
project(
    RVkGaussEngine
	LANGUAGES CXX
    VERSION 0.0.1
)

# Allow users to specify Vulkan SDK path via CMake UI
# Set a cache variable for the Vulkan SDK path, defaulting to an empty string
set(VULKAN_SDK_PATH "" CACHE PATH "Path to the Vulkan SDK installation directory")

include(GNUInstallDirs)

add_subdirectory(Vk_GaussianSplatViewer)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Define the source directory using the correct folder name 'Vk_GaussianSplatViewer'
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Vk_GaussianSplatViewer)

# Make the "Vk_GaussianSplatViewer" source folder an include root
target_include_directories(Vk_GaussianSplatViewer PUBLIC
    ${SRC_DIR}   # this is ${CMAKE_CURRENT_SOURCE_DIR}/Vk_GaussianSplatViewer
)

# Include the generated file in the project
include_directories(${CMAKE_BINARY_DIR})

# Define EXT_DIR for libraries kept in the external folder
set(EXT_DIR ${CMAKE_SOURCE_DIR}/ext)

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
        "${VULKAN_SDK_PATH}/Include/vulkan"
    )

    # Add library directories and link libraries
    # Assuming Windows library structure with .lib files
    set(VULKAN_SDK_LIB_DIR "${VULKAN_SDK_PATH}/lib")

    target_link_libraries(Vk_GaussianSplatViewer
		PUBLIC
        # Vulkan main library (for Vulkan-1.dll)
        "${VULKAN_SDK_LIB_DIR}/vulkan-1.lib"
    )

else()
    message(FATAL_ERROR "VULKAN_SDK_PATH is not set. Please set it in the CMake configuration to point to your Vulkan SDK installation.")
endif()