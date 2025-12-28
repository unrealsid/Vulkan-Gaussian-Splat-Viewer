set(
    Header_Files
	"include/GaussianSplatPly.h"
		include/structs/Gaussian.h

)

source_group(
	TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" 
	PREFIX "Header Files" 
	FILES ${Header_Files}
)

set(
    Source_Files
	"source/GaussianSplatPly.cpp"
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

set(GAUSSIAN_LOADER_PROJECT_NAME "GaussianSplatLoader")

add_executable(
    ${GAUSSIAN_LOADER_PROJECT_NAME} ${ALL_FILES}
)

target_include_directories(
    ${GAUSSIAN_LOADER_PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(NOT TARGET Vk_GaussianSplatViewer::Vk_GaussianSplatViewer)
	message(SEND_ERROR "Attempt to link to non-existent Vk_GaussianSplatViewer.")
endif()

target_link_libraries(${GAUSSIAN_LOADER_PROJECT_NAME} PRIVATE
													Vk_GaussianSplatViewer::Vk_GaussianSplatViewer
													tinyply)