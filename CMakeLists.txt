cmake_minimum_required(VERSION 3.10)

project(starwell VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:lPrimemaster/stperf#master")

CPMAddPackage("gh:glfw/glfw#3.4")

CPMAddPackage(
    NAME glad
    GITHUB_REPOSITORY Dav1dde/glad
    GIT_TAG glad2
)

CPMAddPackage("gh:orconut/imgui@1.90.4-docking")

add_subdirectory(${glad_SOURCE_DIR}/cmake glad_cmake)

glad_add_library(glad_gl_core_45 REPRODUCIBLE API gl:core=4.5)

add_executable(starwell main.cpp)
target_include_directories(starwell PRIVATE ${stperf_SOURCE_DIR} ${glfw_SOURCE_DIR}/include/GLFW ${imgui_SOURCE_DIR})
target_link_libraries(starwell PRIVATE stperf glad_gl_core_45 glfw)
target_compile_options(starwell PRIVATE -Wall -Wextra -Wno-missing-braces -O3)
set_property(TARGET starwell PROPERTY CXX_STANDARD 20)
