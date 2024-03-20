#include "engine/camera.h"
#include "engine/scene/cube.h"
#include "engine/scene/square.h"
#include "engine/shaderprogram.h"

#include <SDL_events.h>
#include <SDL_video.h>

class Editor {
public:
  Editor();
  ~Editor();

  int initialize(SDL_Window *window, SDL_GLContext gl_context);
  void resize(int width, int height);
  void paint();
  void processEvent(const SDL_Event &event);

private:
  SDL_Window *mp_window;
  int m_width;
  int m_height;

  GLuint vao;

  ShaderProgram m_prog_flat;
  ShaderProgram m_prog_lambert;
  Square m_square;
  Cube m_cube;

  Camera m_camera;
};