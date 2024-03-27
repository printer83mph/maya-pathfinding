#pragma once

#include "../drawable.h"

#include <GL/glew.h>

class Cube : public Drawable {
public:
  Cube();
  Cube(float x, float y, float z);
  ~Cube();

  void create() override;

private:
  float x, y, z;
};
