#pragma once

#include <glm/vec2.hpp>

class VelocityObstacle {
public:
  VelocityObstacle(const glm::vec2 &pA, const glm::vec2 &pB,
                   const glm::vec2 &vB, float radiusA, float radiusB);

  bool isInVO(const glm::vec2 &velocityA) const;

private:
  glm::vec2 m_posA, m_origin, m_ray;
  float m_halfConeAngle;
};

class ReciprovalVelocityObject {
public:
  ReciprovalVelocityObject(const glm::vec2 &pA, const glm::vec2 &vA,
                           const glm::vec2 &pB, const glm::vec2 &vB,
                           float radiusA, float radiusB);

  bool isInRVO(const glm::vec2 velocityA) const;

private:
  VelocityObstacle m_aVO, m_bVO;
};
