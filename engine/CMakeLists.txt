cmake_minimum_required(VERSION 3.30)
project(spear)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_compile_options(-Wall -Wextra -pedantic -std=c++20)

set(SPEAR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
add_compile_definitions(SPEAR_ROOT="${SPEAR_ROOT}")

set(SOURCES
    #src/material.cc

    src/window.cc
    src/transform.cc
    src/event_handler.cc
    src/mesh.cc
    src/camera.cc
    src/sprite3d.cc

    # Rendering
    src/rendering/api.cc
    src/rendering/base_shader.cc
    src/rendering/base_texture.cc
    src/rendering/shader_type.cc

    # Rendering Shapes
    src/rendering/shapes/shape.cc
    src/rendering/shapes/base_cube.cc

    # Rendering OpenGL
    src/rendering/opengl/renderer.cc
    src/rendering/opengl/shader.cc

    # Rendering OpenGL Texture
    src/rendering/opengl/texture/texture.cc
    src/rendering/opengl/texture/sdl_texture.cc
    src/rendering/opengl/texture/stb_texture.cc

    # Rendering OpenGL Shapes
    src/rendering/opengl/shapes/cube.cc
    src/rendering/opengl/shapes/quad.cc
)

include_directories(${SDL_INCLUDE_DIRS} ${SDL3_IMAGE_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

add_library(spear ${SOURCES})
target_include_directories(spear PUBLIC include)
target_link_libraries(spear
    PRIVATE SDL3::SDL3 SDL3_image::SDL3_image
    PRIVATE OpenGL::GL GLEW::GLEW
)
