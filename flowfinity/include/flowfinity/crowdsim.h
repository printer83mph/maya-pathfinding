#pragma once

#include "navigation/navmethod.h"

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

  // (DON'T USE) Runs time step without any navigation logic
  void performTimeStep(float dt);
  // Runs time step with path-following logic based on the provided navMethod
  void performTimeStep(float dt, NavMethod* navMethod);

  void addAgent(const glm::vec2& pos, const glm::vec2& target);
  void removeAgent(int index);
  void setAgentCurrentTarget(int index, const glm::vec2& target);
  void setAgentFinalTarget(int index, const glm::vec2& target);

private:
  struct Config {
    // per-agent config
    float maxSpeed = 1.f;
    float acceleration = 20.f;
    float drag = 12.f;
    float radius = 0.25f;
    float aggressiveness = 25.f;

    // spawn/despawn logic
    float spawnRate = .5f;
    int maxAgents = 100;

    // Position pairs representing inflow and outflow for agents.
    std::vector<std::pair<glm::vec2, glm::vec2>> inOutFlows;
  } m_config;

  std::vector<glm::vec2> m_rvoPos;
  std::vector<glm::vec2> m_rvoVel;
  std::vector<glm::vec2> m_rvoCurrentTarget;
  std::vector<glm::vec2> m_rvoFinalTarget;

  std::array<glm::vec2, 33> m_possibleAccels;

  void computeCurrentTarget(int index, NavMethod* navMethod);
  glm::vec2 findOptimalAcceleration(int index, float dt) const;

  void addAgent(float x, float y);
  void addAgent(const glm::vec2& pos);
  void removeAgent(size_t index);
};
