set(SOURCES
  "src/flowfinity.cpp"
  "src/obstacle.cpp"
)

set(HEADERS
  "include/flowfinity.h"
  "include/obstacle.h"
)

add_library(flowfinity STATIC ${SOURCES} ${HEADERS})
target_include_directories(flowfinity PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(flowfinity PRIVATE
  glm::glm
)
