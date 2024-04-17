set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

find_package(Maya REQUIRED)

set(SOURCES
    src/FlowFinityNode.cpp
    src/FlowFinityCmd.cpp
    src/PluginMain.cpp
)

set(HEADERS
    src/FlowFinityNode.h
    src/FlowFinityCmd.h
)

add_library(flowfinityMaya SHARED ${SOURCES} ${HEADERS})

target_include_directories(flowfinityMaya PRIVATE
    ${LSystem_SOURCE_DIR}
    Maya::Maya
    src
)

target_link_libraries(flowfinityMaya PRIVATE
    flowfinity
    Maya::Maya
)

set_property(TARGET flowfinityMaya PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL"
)

MAYA_PLUGIN(flowfinityMaya)

install(TARGETS flowfinityMaya ${MAYA_TARGET_TYPE} DESTINATION plug-ins/${MAYA_VERSION})