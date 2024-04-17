find_package(glm CONFIG REQUIRED)

set(SOURCES
  src/navigation/obstacle.cpp
  src/navigation/visibilitygraph.cpp
  src/util/graph.cpp
  src/flowfinity.cpp
  src/rvo.cpp
)

set(HEADERS
  include/flowfinity/flowfinity.h
  include/flowfinity/navigation/visibilitygraph.h
  include/flowfinity/navigation/obstacle.h
  include/flowfinity/util/graph.h
  include/flowfinity/rvo.h
)

add_library(flowfinity STATIC ${SOURCES} ${HEADERS})

target_include_directories(flowfinity PUBLIC
	include
	${GLM_INCLUDE_DIRS}
)

target_include_directories(flowfinity PRIVATE
	src
)

target_link_libraries(flowfinity PUBLIC
	glm::glm
)
