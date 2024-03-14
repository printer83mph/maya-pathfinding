target_sources(flowfinityGl PRIVATE
  editor.cpp
  editor.h
  main.cpp
)

target_include_directories(flowfinityGl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(engine)
