#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#undef GLM_DISABLE_EXPERIMENTAL
#include <glm/vec2.hpp>

#include <array>
#include <vector>

/**
 * Class representing a crowd simulation instance
 */
class CrowdSim
{
public:
  CrowdSim();
  ~CrowdSim();

  int size() const;
  const std::vector<glm::vec2>& getAgentPositions() const;
  const std::vector<glm::vec2>& getAgentVelocities() const;
  const std::vector<glm::vec2>& getAgentTargets() const;

  void performTimeStep(float dt);

  void addAgent(const glm::vec2& pos, const glm::vec2& target);
  void removeAgent(int index);
  void setAgentTarget(int index, const glm::vec2& target);

private:
  struct Config {
    float maxSpeed = 1.f;
    float acceleration = 20.f;
    float drag = 12.f;
    float radius = 0.25f;
    float aggressiveness = 25.f;
  } m_config;

  std::vector<glm::vec2> m_rvoPos;
  std::vector<glm::vec2> m_rvoVel;
  std::vector<glm::vec2> m_rvoTarget;

  std::array<glm::vec2, 33> m_possibleAccels;

  glm::vec2 findOptimalAcceleration(int index, float dt) const;

  void addAgent(float x, float y);
  void addAgent(const glm::vec2& pos);
  void removeAgent(size_t index);
};
