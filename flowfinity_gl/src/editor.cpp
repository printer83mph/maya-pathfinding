#include "editor.h"

#include "engine/scene/square.h"
#include "engine/shaderprogram.h"

#include <glm/gtc/matrix_transform.hpp>

Editor::Editor() : m_square(), m_prog_flat() {}

Editor::~Editor() {}

void Editor::initialize() {
  m_square.create();
  m_prog_flat.create("passthrough.vert.glsl", "flat.frag.glsl");
}

void Editor::paint() {
  m_prog_flat.setModelMatrix(glm::mat4());
  m_prog_flat.setViewProjMatrix(
      glm::perspective<float>(glm::radians(90.), 1., 0.01, 100.) *
      glm::translate(glm::mat4(), glm::vec3({0., 0., -5.})));

  glClearColor(1., 1., 1., 1.);
  glClear(GL_COLOR_BUFFER_BIT);

  m_prog_flat.draw(m_square);
}