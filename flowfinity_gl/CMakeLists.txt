find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

# imgui source files
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extern/imgui)
file(
  GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/*.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp # for using std::string
)

# main executable
add_executable(flowfinityGl)

# source files
add_subdirectory(src)
target_sources(flowfinityGl PRIVATE ${IMGUI_SOURCES})

# include dirs
target_include_directories(flowfinityGl PRIVATE
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
  ${SDL2_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
)

# link libraries
target_link_libraries(flowfinityGl PRIVATE
  flowfinity
  ${SDL2_LIBRARIES}
  GLEW::glew
  ${OPENGL_LIBRARIES}
  glm::glm
)

# copy resources to build dir
add_subdirectory(resources)

# copy sdl2 debug targets to output dir
if (WIN32)
    add_custom_command(
        TARGET flowfinityGl POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:flowfinityGl>"
        VERBATIM
    )
    add_custom_command(
        TARGET flowfinityGl POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:GLEW::glew>" "$<TARGET_FILE_DIR:flowfinityGl>"
        VERBATIM
    )
endif()