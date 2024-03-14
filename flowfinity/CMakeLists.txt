set(SOURCES
  "src/flowfinity.cpp"
)

set(HEADERS
  "include/flowfinity.h"
)

add_library(flowfinity STATIC ${SOURCES} ${HEADERS})
target_include_directories(flowfinity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
