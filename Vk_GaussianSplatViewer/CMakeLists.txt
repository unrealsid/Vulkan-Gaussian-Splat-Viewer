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

set(ENGINE_PROJECT_NAME "Vk_GaussianSplatViewer")

add_executable(
    ${ENGINE_PROJECT_NAME} ${ALL_FILES}
) 

target_include_directories(
    ${ENGINE_PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE )

#----------------------------------------------
# Configure VK Bootstrapper from github
# Library used to streamline Vulkan setup
#----------------------------------------------

# Try to find an installed vk-bootstrap first
find_package(vk-bootstrap QUIET)

message(STATUS "vk-bootstrap initializing....")

include(FetchContent)

set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

FetchContent_Declare(
	fetch_vk_bootstrap
	GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
	GIT_TAG        v1.4.320 #Might need to change this version number 
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)

# Link to it (works in both cases)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE vk-bootstrap::vk-bootstrap)