#pragma once

#include "../drawable.h"

#include <GL/glew.h>

class Square : public Drawable
{
public:
  Square();
  ~Square();

  void create() override;
};