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
  std::vector<glm::vec2> m_rvoPos;
  std::vector<glm::vec2> m_rvoVel;
  std::vector<unsigned int> m_rvoTarget;

  std::vector<glm::vec2> m_targets;

  void addAgent(const glm::vec2 &pos, unsigned int target = 0);
  void removeAgent(unsigned int index);
};
