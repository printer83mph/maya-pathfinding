add_custom_target(resources DEPENDS
  glsl/flat.frag.glsl
  glsl/passthrough.vert.glsl
)

add_custom_command(TARGET resources POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}
  $<TARGET_FILE_DIR:flowfinityGl>/resources
)

add_dependencies(flowfinityGl
  resources
)
