cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_PARALLEL_LEVEL 3)
project(flowfinity_gl)

# Prereq libraries
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# normal source files
file(GLOB sources src/*.cpp)

# imgui source files
set(IMGUI_DIR ${VENDOR_DIR}/imgui)
file(
  GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/*.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp # for using std::string
)
list(APPEND sources ${IMGUI_SOURCES})

set(APP_EXE ${PROJECT_NAME})

# main executable
add_executable(${APP_EXE} src/main.cpp ${sources})
# link libraries
target_link_libraries(
  ${APP_EXE}
  # TODO
  # flowfinity_lib
  ${SDL2_LIBRARIES}
  ${OPENGL_LIBRARIES}
)
# include dirs
target_include_directories(
  ${APP_EXE}
  PRIVATE ${IMGUI_DIR}
  PRIVATE ${IMGUI_DIR}/backends
  PRIVATE ${OPENGL_INCLUDE_DIR}
  PRIVATE ${SDL2_INCLUDE_DIR}
)