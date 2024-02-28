cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
project(flowfinity)

set(SOURCES
  "src/flowfinity.cpp"
)

set(HEADERS
  "src/flowfinity.h"
)

add_library(flowfinity STATIC ${SOURCES} ${HEADERS})
