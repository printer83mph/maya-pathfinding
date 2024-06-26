#include "editor.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include <cstdlib>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#undef GLM_ENABLE_EXPERIMENTAL
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_video.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Editor::Editor()
    : m_square(), m_cube(), m_prog_flat(), m_prog_lambert(), m_pathDisplay(), m_flowFinity(),
      m_cubeTransforms(), m_visgraph(), m_paths(), m_camera(), m_drawPath(false),
      m_graphCreated(false)
{
}

Editor::~Editor()
{
  glDeleteVertexArrays(1, &vao);
  m_square.destroy();
}

void Editor::addCubeObstacle(glm::vec2 translation, glm::vec2 scale, float rotation)
{
  // Record Cube Transforms for drawing later
  m_cubeTransforms.push_back({translation, scale, rotation});

  m_visgraph.addCubeObstacle(translation, scale, rotation);
}

void Editor::loadTestScene()
{
  clearObstacles();
  // Clear paths
  m_paths.clear();
  m_pathDisplay.clear();
  m_drawPath = false;
  m_flowFinity.m_config.inOutFlows.clear();

  addCubeObstacle(glm::vec2(0, 0), glm::vec2(3, 2), 63);
  addCubeObstacle(glm::vec2(-7, -8), glm::vec2(3, 2), 215);
  addCubeObstacle(glm::vec2(-4, -4), glm::vec2(1, 2), 157);
  addCubeObstacle(glm::vec2(6, 6), glm::vec2(4, 2), 172);
  addCubeObstacle(glm::vec2(-6, 6), glm::vec2(1, 3), 97);
  addCubeObstacle(glm::vec2(6, -5), glm::vec2(2, 5), 58);
}

void Editor::clearObstacles()
{
  m_visgraph.clearObstacles();
  m_graphCreated = false;
  m_cubeTransforms.clear();
}

void Editor::addActors(int numAgents)
{
  // initialize some debug flowFinity agents
  float angle = 0.f;
  for (int i = 0; i < numAgents; ++i) {
    angle += glm::two_pi<float>() / (float)numAgents;
    float cx = glm::cos(angle) * 2.f;
    float sx = glm::sin(angle) * 2.f;
    m_flowFinity.addAgent(glm::vec2(cx, sx), glm::vec2(-cx, -sx));
  }
}

void Editor::createGraph()
{
  m_paths.clear();
  m_pathDisplay.clear();
  m_drawPath = false;
  m_visgraph.createGraph();
  m_graphCreated = true;
}

void Editor::getDisjkstraPath(std::vector<std::pair<glm::vec3, glm::vec3>> endpoints)
{
  // Create graph just in case there are other obstacles that havent been added to the graph yet
  createGraph();

  if (m_drawPath) {
    m_visgraph.clearEndPoints();
    m_paths.clear();
    m_pathDisplay.clear();
  }

  for (auto& inOutPair : endpoints) {
    auto path = m_visgraph.getPath(glm::vec2(inOutPair.first.x, inOutPair.first.z),
                                   glm::vec2(inOutPair.second.x, inOutPair.second.z));
    auto path_vec3 = std::vector<glm::vec3>();
    for (auto& point : path) {
      path_vec3.push_back({point.x, 0, point.y});
    }
    m_paths.push_back(path_vec3);
    glm::vec3 first = {inOutPair.first.x, 0, inOutPair.first.z};
    glm::vec3 second = {inOutPair.second.x, 0, inOutPair.second.z};
    m_flowFinity.m_config.inOutFlows.push_back({{first.x, first.z}, {second.x, second.z}});
  }

  std::vector<glm::vec3> colors = {glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)};
  int i = 0;
  for (auto& path : m_paths) {
    m_pathDisplay.push_back(PathDisplay(path));
    m_pathDisplay.back().setColor(colors[i]);
    m_pathDisplay.back().create();
    i++;
  }
  m_drawPath = true;
}

int Editor::initialize(SDL_Window* window, SDL_GLContext gl_context)
{
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
  glEnable(GL_COLOR_MATERIAL);
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

  m_cube.create();
  m_square.create();

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

void Editor::update(float dt)
{
  // Run time step for simulation
  m_flowFinity.performTimeStep(dt, &m_visgraph);
}

void Editor::paint()
{
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

  // Draw ground
  m_prog_flat.setModelMatrix(
      glm::scale(glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)), glm::vec3(10, 10, 0)));
  m_prog_flat.draw(m_square);

  // Draw obstacles based on cube transform list
  for (auto& cubeTransform : m_cubeTransforms) {
    m_prog_lambert.setModelMatrix(
        glm::translate(glm::mat4(1.0f),
                       glm::vec3(cubeTransform.translation.x, 1.f, cubeTransform.translation.y)) *
        glm::rotate(glm::radians(cubeTransform.rotation), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(cubeTransform.scale.x, 1, cubeTransform.scale.y)));
    m_prog_lambert.draw(m_cube);
  }

  // Draw obstacle vertice
  glDisable(GL_DEPTH_TEST);
  m_prog_flat.setModelMatrix(glm::mat4(1.f));

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glColor4f(0, 1, 0, 1);
  glBegin(GL_POINTS);
  for (auto& obstacle : m_visgraph.getObstacles()) {
    for (auto& bound : obstacle.getBounds()) {
      glVertex3f(bound.point2.x, 0, bound.point2.z);
    }
  }
  glColor4f(0, 1, 0, 1);
  glEnd();

  if (m_graphCreated && !m_drawPath) {
    glBegin(GL_LINES);
    for (auto& edge : m_visgraph.getEdges()) {
      glColor3f(0, 1, 0);
      glVertex3f(edge.first.x, 0, edge.first.y);
      glVertex3f(edge.second.x, 0, edge.second.y);
    }
    glEnd();
    // update(0.01f);
  }

  if (m_drawPath) {
    for (auto& path : m_pathDisplay) {
      m_prog_flat.draw(path);
    }
  }

  // draw agents
  {
    auto& agentPos = m_flowFinity.getAgentPositions();
    auto& agentVel = m_flowFinity.getAgentVelocities();

    // draw positions
    glBegin(GL_POINTS);
    for (auto& pos : agentPos) {
      glVertex3f(pos.x, 0, pos.y);
    }
    glEnd();

    // draw velocities
    glBegin(GL_LINES);
    for (int i = 0; i < m_flowFinity.size(); ++i) {
      const auto& pos = agentPos.at(i);
      const auto& vel = agentVel.at(i);
      auto end = pos + vel * 0.5f;
      glColor3f(0, 0, 1);
      glVertex3f(pos.x, 0, pos.y);
      glColor3f(0, 0, 1);
      glVertex3f(end.x, 0, end.y);
    }
    glEnd();
  }
}

void Editor::processEvent(const SDL_Event& event)
{
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
