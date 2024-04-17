#pragma once

#include "engine/camera.h"
#include "engine/scene/cube.h"
#include "engine/scene/pathDisplay.h"
#include "engine/scene/square.h"
#include "engine/shaderprogram.h"
#include "flowfinity/flowfinity.h"
#include "flowfinity/navigation/visibilitygraph.h"

#include "glm/ext/vector_float2.hpp"

#include <SDL_events.h>
#include <SDL_video.h>

struct CubeTransform {
  glm::vec2 translation;
  glm::vec2 scale;
  float rotation;
};

class Editor
{
public:
  Editor();
  ~Editor();

  int initialize(SDL_Window* window, SDL_GLContext gl_context);
  void resize(int width, int height);
  void update(float dt);
  void paint();
  void processEvent(const SDL_Event& event);
  void addCubeObstacle(glm::vec2 translation, glm::vec2 scale, float rotation);
  void addActors(int numAgents);
  void createGraph();
  void getDisjkstraPath(std::vector<std::pair<glm::vec3, glm::vec3>> endPoints);
  void clearObstacles();

private:
  SDL_Window* mp_window;
  int m_width;
  int m_height;

  GLuint vao;

  FlowFinity m_flowFinity;
  VisibilityGraph m_visgraph;

  ShaderProgram m_prog_flat;
  ShaderProgram m_prog_lambert;
  Square m_square;
  Cube m_cube;
  std::vector<PathDisplay> m_pathDisplay;

  bool m_drawPath;
  bool m_graphCreated;

  std::vector<CubeTransform> m_cubeTransforms;
  std::vector<std::vector<glm::vec3>> m_paths;

  Camera m_camera;
};
