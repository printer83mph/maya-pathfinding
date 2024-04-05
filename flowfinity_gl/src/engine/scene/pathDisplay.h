#pragma once

#include "../drawable.h"
#include "glm/ext/vector_float3.hpp"

#include <GL/glew.h>
#include <vector>

class PathDisplay : public Drawable {
public:
  PathDisplay();
  PathDisplay(const std::vector<glm::vec3> &path);
  ~PathDisplay();

  void create() override;
  GLenum drawMode() override;

private:
  std::vector<glm::vec3> m_path;
};
