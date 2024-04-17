#pragma once

#include <glm/vec2.hpp>
#include <vector>

class NavMethod
{
public:
  NavMethod();
  virtual ~NavMethod();

  virtual std::vector<glm::vec2> getPath(const glm::vec2& start, const glm::vec2& end) = 0;
};
