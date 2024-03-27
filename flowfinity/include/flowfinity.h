#pragma once

#include <array>
#include <glm/vec2.hpp>

#include <vector>

/**
 * Class representing a crowd simulation instance
 */
class FlowFinity {
public:
  FlowFinity();
  ~FlowFinity();

  struct Config {
    float maxSpeed = 1.f;
    float acceleration = 0.2f;
    float radius = 0.25f;
    float aggressiveness = 1.f;
  } m_config;

  int size() const;

  void performTimeStep(float dt);
  const std::vector<glm::vec2> &getAgentPositions() const;

private:
  std::vector<glm::vec2> m_rvoPos;
  std::vector<glm::vec2> m_rvoVel;
  std::vector<glm::vec2> m_rvoTarget;

  std::array<glm::vec2, 8> m_possibleAccels;

  void addAgent(const glm::vec2 &pos, glm::vec2 &target);
  void removeAgent(int index);

  glm::vec2 findOptimalAcceleration(int index, float dt) const;
};
