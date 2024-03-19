#pragma once

#include <glm/vec2.hpp>

#include <vector>

/**
 * Class representing a crowd simulation instance
 */
class FlowFinity {
public:
  FlowFinity();
  ~FlowFinity();

  void performTimeStep(float dt);

private:
  std::vector<glm::vec2> m_pos;
  std::vector<glm::vec2> m_vel;

  void addAgent(float x, float y);
  void addAgent(const glm::vec2 &pos);
  void removeAgent(uint index);
};