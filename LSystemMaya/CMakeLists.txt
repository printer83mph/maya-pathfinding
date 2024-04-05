cmake_minimum_required(VERSION 3.18)

project(LSystemMaya)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add source files to the project
set(SOURCES
src/cylinder.cpp
${LSystem_SOURCE_DIR}/LSystem.cpp
src/LSystemCmd.cpp
src/PluginMain.cpp
${LSystem_SOURCE_DIR}/vec.cpp
)


# Add header files to the project
set(HEADERS
    src/cylinder.h
    ${LSystem_SOURCE_DIR}/LSystem.h
    src/LSystemCmd.h
    ${LSystem_SOURCE_DIR}/matrix.h
    ${LSystem_SOURCE_DIR}/vec.h
)

find_package(Maya REQUIRED)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Maya::Maya)
target_include_directories(${PROJECT_NAME} 
    PUBLIC ${LSystem_SOURCE_DIR}
    PRIVATE Maya::Maya
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
)
MAYA_PLUGIN(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} ${MAYA_TARGET_TYPE} DESTINATION plug-ins/${MAYA_VERSION})