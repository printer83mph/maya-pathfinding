#include "engine/scene/square.h"
#include "engine/shaderprogram.h"

#include <SDL_video.h>

class Editor {
public:
  Editor();
  ~Editor();

  int initialize(SDL_Window *window, SDL_GLContext gl_context);
  void paint();

private:
  ShaderProgram m_prog_flat;
  Square m_square;

  GLuint vao;
};