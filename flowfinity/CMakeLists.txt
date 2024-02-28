cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
project(flowfinity)

file(GLOB SOURCES src/*.cpp)

file(GLOB HEADERS include/*.h)

add_library(flowfinity STATIC ${SOURCES} ${HEADERS})
target_include_directories(flowfinity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
