cmake_minimum_required(VERSION 3.10)

project(starwell VERSION 0.1 LANGUAGES CXX)

add_executable(starwell main.cpp)
target_compile_options(starwell PRIVATE -Wall -Wextra -Og)
