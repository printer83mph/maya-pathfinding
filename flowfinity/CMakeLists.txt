find_package(glm CONFIG REQUIRED)

set(SOURCES
  src/flowfinity.cpp
  src/graph.cpp
  src/obstacle.cpp
  src/rvo.cpp
)

set(HEADERS
  include/flowfinity.h
  include/graph.h
  include/obstacle.h
  include/rvo.h
)

add_library(flowfinity STATIC ${SOURCES} ${HEADERS})

target_include_directories(flowfinity PUBLIC
	include
	${GLM_INCLUDE_DIRS}
)

target_link_libraries(flowfinity PUBLIC
	glm::glm
)
