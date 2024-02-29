#include "editor.h"

#include "engine/scene/square.h"
#include "engine/shaderprogram.h"

#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Editor::Editor() : m_square(), m_prog_flat() {}

Editor::~Editor() {
  glDeleteVertexArrays(1, &vao);
  m_square.destroy();
}

int Editor::initialize(SDL_Window *window, SDL_GLContext gl_context) {
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cout << "Failed to init GLEW" << std::endl;
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Set a few settings/modes in OpenGL rendering
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  // Set the size with which points should be rendered
  glPointSize(5);
  // Set the color with which the screen is filled at the start of each render
  // call.
  glClearColor(0.25, 0.25, 0.25, 1);

  // Create a Vertex Attribute Object
  glGenVertexArrays(1, &vao);

  m_square.create();
  m_prog_flat.create("passthrough.vert.glsl", "flat.frag.glsl");

  // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
  // using multiple VAOs, we can just bind one once.
  glBindVertexArray(vao);

  return 0;
}

void Editor::paint() {
  m_prog_flat.setModelMatrix(glm::mat4(1));
  m_prog_flat.setViewProjMatrix(
      glm::perspective<float>(45., 1., 0.01, 100.) *
      glm::translate(glm::mat4(), glm::vec3({0., 0., -5.})));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_prog_flat.draw(m_square);
}