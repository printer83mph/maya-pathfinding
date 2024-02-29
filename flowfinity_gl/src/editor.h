#include "engine/scene/square.h"
#include "engine/shaderprogram.h"

class Editor {
public:
  Editor();
  ~Editor();

  void initialize();
  void paint();

private:
  ShaderProgram m_prog_flat;
  Square m_square;
};