#include "glm/ext/matrix_transform.hpp"
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include "editor.h"

#include "glm/gtx/transform.hpp"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_video.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Editor::Editor()
    : m_square(), m_cube(), m_prog_flat(), m_prog_lambert(), m_obstacles(),
      m_cubeTransforms(), m_camera() {}

Editor::~Editor() {
  glDeleteVertexArrays(1, &vao);
  m_square.destroy();
}

void Editor::addCubeObstacle(glm::vec2 translation, glm::vec2 scale,
                             float rotation) {
  // Record Cube Transforms for drawing later
  m_cubeTransforms.push_back({translation, scale, rotation});

  // Default cube vertices
  std::vector<glm::vec2> pos_data{glm::vec2(-1, -1), glm::vec2(-1, 1),
                                  glm::vec2(1, 1), glm::vec2(1, -1)};

  // Set cube transformations
  glm::mat2 scale_mat = glm::mat2(scale.x, 0, 0, scale.y);
  glm::mat2 rot = glm::mat2(
      glm::cos(glm::radians(rotation)), -glm::sin(glm::radians(rotation)),
      glm::sin(glm::radians(rotation)), glm::cos(glm::radians(rotation)));

  // Apply transformations to cube vertices
  for (int i = 0; i < 4; i++) {
    pos_data[i] = (rot * scale_mat * pos_data[i]) + translation;
  }

  // Create obstacle from transformed cube vertices
  std::unique_ptr<Obstacle> obstacle = std::make_unique<Obstacle>();
  obstacle->addBound(Edge{glm::vec3(pos_data[0][0], 0, pos_data[0][1]),
                          glm::vec3(pos_data[1][0], 0, pos_data[1][1])});
  obstacle->addBound(Edge{glm::vec3(pos_data[1][0], 0, pos_data[1][1]),
                          glm::vec3(pos_data[2][0], 0, pos_data[2][1])});
  obstacle->addBound(Edge{glm::vec3(pos_data[2][0], 0, pos_data[2][1]),
                          glm::vec3(pos_data[3][0], 0, pos_data[3][1])});
  obstacle->addBound(Edge{glm::vec3(pos_data[3][0], 0, pos_data[3][1]),
                          glm::vec3(pos_data[0][0], 0, pos_data[0][1])});
  m_obstacles.push_back(std::move(obstacle));
}

int Editor::initialize(SDL_Window *window, SDL_GLContext gl_context) {
  mp_window = window;

  SDL_GL_GetDrawableSize(window, &m_width, &m_height);
  m_camera = Camera(m_width, m_height);
  m_camera.ScaleZoom(3);
  m_camera.RotateAboutRight(-45);
  m_camera.RotateAboutUp(-45);
  m_camera.RecomputeAttributes();

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
  m_cube.create();

  m_prog_flat.create("passthrough.vert.glsl", "flat.frag.glsl");
  m_prog_lambert.create("passthrough.vert.glsl", "lambert.frag.glsl");

  // Add some obstacles
  // addCubeObstacle(glm::vec2(1, 2), glm::vec2(2, 2), 45);
  // addCubeObstacle(glm::vec2(-5, -2), glm::vec2(1, 3), 15)

  // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
  // using multiple VAOs, we can just bind one once.
  glBindVertexArray(vao);

  return 0;
}

void Editor::paint() {
  m_prog_flat.setModelMatrix(glm::mat4(1.f));
  m_prog_flat.setViewProjMatrix(m_camera.getViewProj());

  m_prog_lambert.setCamPos(m_camera.eye);
  m_prog_lambert.setModelMatrix(glm::mat4(1.f));
  m_prog_lambert.setViewProjMatrix(m_camera.getViewProj());

  SDL_GL_GetDrawableSize(mp_window, &m_width, &m_height);
  m_camera.width = m_width;
  m_camera.height = m_height;
  glViewport(0, 0, m_width, m_height);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_prog_flat.setModelMatrix(
      glm::scale(glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)),
                 glm::vec3(10, 10, 0)));
  m_prog_flat.draw(m_square);

  for (auto &cubeTransform : m_cubeTransforms) {
    m_prog_lambert.setModelMatrix(
        glm::translate(glm::mat4(1.0f),
                       glm::vec3(cubeTransform.translation.x, 1.f,
                                 cubeTransform.translation.y)) *
        glm::rotate(glm::radians(cubeTransform.rotation), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f),
                   glm::vec3(cubeTransform.scale.x, 1, cubeTransform.scale.y)));
    m_prog_lambert.draw(m_cube);
  }

  glDisable(GL_DEPTH_TEST);
  m_prog_flat.setModelMatrix(glm::mat4(1.f));

  glBegin(GL_POINTS);
  for (auto &obstacle : m_obstacles) {
    for (auto &bound : obstacle->getBounds()) {
      glVertex3f(bound.point2.x, 0, bound.point2.z);
    }
  }
  glEnd();
}

void Editor::processEvent(const SDL_Event &event) {
  switch (event.type) {
  case SDL_MOUSEMOTION:
    if (event.motion.state & SDL_BUTTON_LMASK) {
      m_camera.RotateAboutUp(-event.motion.xrel * 0.25);
      m_camera.RotateAboutRight(-event.motion.yrel * 0.25);
      m_camera.RecomputeAttributes();
    } else if (event.motion.state & SDL_BUTTON_RMASK) {
      m_camera.ScaleZoom(1. + event.motion.yrel * 0.005);
      m_camera.RecomputeAttributes();
    }
    break;
  case SDL_MOUSEWHEEL:
    m_camera.ScaleZoom(1. - event.wheel.y * 0.1);
    m_camera.RecomputeAttributes();
    break;
  }
}