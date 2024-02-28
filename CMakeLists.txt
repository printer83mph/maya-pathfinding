cmake_minimum_required(VERSION 3.24)

project(maya_pathfinding)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

# Backend logic
add_subdirectory(flowfinity)
add_subdirectory(flowfinity_gl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
