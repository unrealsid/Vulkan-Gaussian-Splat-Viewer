include(FetchContent)

find_package(Vulkan REQUIRED)

find_package(vk-bootstrap QUIET)
if(vk-bootstrap_FOUND)
    message(STATUS "Using vk-bootstrap via find_package")
endif()

if(NOT vk-bootstrap_FOUND)
	FetchContent_Declare(
		vk_bootstrap
		GIT_REPOSITORY "https://github.com/charles-lunarg/vk-bootstrap"
		GIT_TAG        v1.4.321
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	message(STATUS "Using vk-bootstrap via FetchContent")
	FetchContent_MakeAvailable(vk_bootstrap)
endif()

find_package(SDL3 CONFIG QUIET)
if(SDL3_FOUND)
    message(STATUS "Using SDL3 via find_package")
endif()
if(NOT SDL3_FOUND)
	FetchContent_Declare(
		SDL3
		GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
		GIT_TAG        "main"
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	message(STATUS "Using SDL3 via FetchContent")
	FetchContent_MakeAvailable(SDL3)
endif()

find_package(STB QUIET)
if(STB_FOUND)
    message(STATUS "Using STB via find_package")
endif()
if(NOT STB_FOUND)
	FetchContent_Declare(
		STB
		GIT_REPOSITORY "https://github.com/unrealsid/stb-cmake.git"
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	message(STATUS "Using STB via FetchContent")
	FetchContent_MakeAvailable(STB)
endif()