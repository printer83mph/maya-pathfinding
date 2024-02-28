cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_PARALLEL_LEVEL 3)
project(flowfinity_gl)

# Prereq libraries
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# normal source files
file(GLOB_RECURSE sources src/*.cpp)

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
add_executable(${APP_EXE} ${sources})
# include dirs
target_include_directories(
  ${APP_EXE}
  PRIVATE ${IMGUI_DIR}
  PRIVATE ${IMGUI_DIR}/backends
  PRIVATE ${SDL2_INCLUDE_DIR}
  PRIVATE ${OPENGL_INCLUDE_DIR}
  PRIVATE ${GLEW_INCLUDE_DIR}
)
# link libraries
target_link_libraries(
  ${APP_EXE}
  flowfinity
  ${SDL2_LIBRARIES}
  ${OPENGL_LIBRARIES}
  GLEW::GLEW
)
