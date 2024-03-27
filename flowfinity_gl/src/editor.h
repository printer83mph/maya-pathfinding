#include "engine/camera.h"
#include "engine/scene/cube.h"
#include "engine/scene/square.h"
#include "engine/shaderprogram.h"
#include "flowfinity.h"
#include "glm/ext/vector_float2.hpp"
#include "obstacle.h"

#include <flowfinity.h>

#include <SDL_events.h>
#include <SDL_video.h>

struct CubeTransform {
  glm::vec2 translation;
  glm::vec2 scale;
  float rotation;
};

class Editor {
public:
  Editor();
  ~Editor();

  int initialize(SDL_Window *window, SDL_GLContext gl_context);
  void resize(int width, int height);
  void update(float dt);
  void paint();
  void processEvent(const SDL_Event &event);
  void addCubeObstacle(glm::vec2 translation, glm::vec2 scale, float rotation);
  void addActors(int numAgents);
  void createGraph();

private:
  SDL_Window *mp_window;
  int m_width;
  int m_height;

  GLuint vao;

  FlowFinity m_flowFinity;

  ShaderProgram m_prog_flat;
  ShaderProgram m_prog_lambert;
  Square m_square;
  Cube m_cube;

  std::vector<Obstacle> m_obstacles;
  std::vector<CubeTransform> m_cubeTransforms;

  Camera m_camera;
};