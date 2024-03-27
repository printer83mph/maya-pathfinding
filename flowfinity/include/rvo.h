#pragma once

#include <glm/vec2.hpp>

class VelocityObstacle {
public:
  VelocityObstacle(const glm::vec2 &pA, const glm::vec2 &vA,
                   const glm::vec2 &pB, const glm::vec2 &vB, float radiusA,
                   float radiusB);

  bool isInVO(const glm::vec2 &newVelA) const;
  bool isInRVO(const glm::vec2 &newVelA) const;

private:
  glm::vec2 m_posA, m_velA, m_rayOrigin, m_rayDir;
  float m_halfConeAngle;
};
