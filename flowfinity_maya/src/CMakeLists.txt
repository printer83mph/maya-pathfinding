target_sources(flowfinityMaya PRIVATE
  FlowFinityNode.cpp
  FlowFinityNode.h
  FlowFinityCmd.cpp
  FlowFinityCmd.h
  PluginMain.cpp
)

target_include_directories(flowfinityMaya PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
