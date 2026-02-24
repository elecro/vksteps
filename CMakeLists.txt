cmake_minimum_required(VERSION 3.20)
project(vksteps LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan 1.1 REQUIRED)

option(BUILD_GLFW "If enabled download and build glfw lib also" OFF)

if(NOT BUILD_GLFW)
    find_package(glfw3 3.3)
endif()

if(NOT glfw3_FOUND)
    message("-- Forcing GLFW3 download and build")
    set(BUILD_GLFW ON)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external)

include(shader.cmake)

add_compile_options(-Wall)
if(NOT WIN32)
	add_compile_options(
	    -Wextra -Werror
        -Wno-error=unused-variable
        -Wno-error=unused-but-set-variable
        -Wno-error=unused-parameter
    )
endif()

#add_subdirectory(lib)

set(NAME app)
add_executable(${NAME}
    main.cpp

    lib/context.cpp
    lib/imgui_integration.cpp
    lib/swapchain.cpp
    lib/wrappers.cpp
)
target_include_directories(${NAME} PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${EXTERNALS_BUILD_DIR}
    lib/
)

target_link_libraries(${NAME}
    PRIVATE glfw Vulkan::Vulkan imgui
)
