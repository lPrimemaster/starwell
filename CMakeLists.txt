cmake_minimum_required(VERSION 3.10)

set(starwell_version "0.2.0")
project(starwell VERSION ${starwell_version} LANGUAGES C CXX)
configure_file(include/config.h.in config.h @ONLY)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:lPrimemaster/stperf#master")

CPMAddPackage("gh:glfw/glfw#3.4")

CPMAddPackage(
    NAME glad
    GITHUB_REPOSITORY Dav1dde/glad
    GIT_TAG glad2
)

CPMAddPackage("gh:ocornut/imgui@1.90.4-docking")

CPMAddPackage("gh:pybind/pybind11@2.11.1")

if(pybind11_ADDED)
    # https://pybind11.readthedocs.io/en/stable/faq.html#someclass-declared-with-greater-visibility-than-the-type-of-its-field-someclass-member-wattributes
    target_compile_options(pybind11::embed INTERFACE -fvisibility=hidden)
endif()

set(IMGUI_EXTRA_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

add_subdirectory(${glad_SOURCE_DIR}/cmake glad_cmake)

# add_subdirectory(${boost_python_SOURCE_DIR})

glad_add_library(glad_gl_core_45 REPRODUCIBLE API gl:core=4.5)

add_executable(starwell
    # Math, Vectors, Matrices
    include/math.h
    src/math.cpp

    # 3D Camera controls
    include/camera.h
    src/camera.cpp

    # A simulated body
    include/body.h
    src/body.cpp

    # The Barnes-Hut algorithm
    include/bhtree.h
    src/bhtree.cpp

    # Rendering
    include/rwindow.h
    src/rwindow.cpp
    include/draw.h
    src/draw.cpp

    # Scenes
    include/scene.h
    src/scene.cpp

    # GUI Windows
    include/windows/window.h
    src/windows/window.cpp
    include/windows/settings.h
    src/windows/settings.cpp

    src/main.cpp
    ${IMGUI_EXTRA_SOURCES}
)

target_include_directories(starwell PRIVATE
    ${stperf_SOURCE_DIR}
    ${glfw_SOURCE_DIR}/include/glfw
    ${imgui_SOURCE_DIR}
    ${pybind_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(starwell PRIVATE stperf glad_gl_core_45 glfw pybind11::embed)
target_compile_options(starwell PRIVATE -Wall -Wextra -Wno-missing-braces -O3)
set_property(TARGET starwell PROPERTY CXX_STANDARD 20)

# Copy to build dir
add_custom_target(
    copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/shaders.glsl
    ${CMAKE_CURRENT_BINARY_DIR}/shaders.glsl
)
add_custom_target(
    copy_scenes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/scenes/
    ${CMAKE_CURRENT_BINARY_DIR}/scenes/
)

add_dependencies(starwell copy_shaders copy_scenes)
