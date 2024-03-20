#pragma once

#include "../drawable.h"

#include <GL/glew.h>

class Cube : public Drawable {
public:
  Cube();
  ~Cube();

  void create() override;
};
