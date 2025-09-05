cmake_minimum_required(
	VERSION 4.0.0 
	FATAL_ERROR
)
project(
    RVkGaussEngine
	LANGUAGES CXX
    VERSION 0.0.1
)


# ------------------------------------------------------
#  1. Add Vulkan and related libraries (vma/glm etc) from Vulkan SDK 
#  I'm assuming we're using GLM, vma etc from the SDK installer
# ------------------------------------------------------

find_package(Vulkan REQUIRED)
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
