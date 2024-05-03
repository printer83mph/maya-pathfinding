find_package(glm CONFIG REQUIRED)

set(SOURCES
  src/navigation/navmethod.cpp
  src/navigation/visibilitygraph.cpp
  src/navigation/obstacle.cpp
  src/util/graph.cpp
  src/encoding.cpp
  src/crowdsim.cpp
  src/rvo.cpp
)

set(HEADERS
  include/flowfinity/navigation/navmethod.h
  include/flowfinity/navigation/visibilitygraph.h
  include/flowfinity/navigation/obstacle.h
  include/flowfinity/util/graph.h
  include/flowfinity/encoding.h
  include/flowfinity/crowdsim.h
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
