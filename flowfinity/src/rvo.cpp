#include "rvo.h"

#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

VelocityObstacle::VelocityObstacle(const glm::vec2 &pA, const glm::vec2 &vA,
                                   const glm::vec2 &pB, const glm::vec2 &vB,
                                   float radiusA, float radiusB)
    : m_posA(pA), m_velA(vA), m_origin(pA + vB), m_ray(glm::normalize(pB - pA)),
      m_halfConeAngle(glm::asin((radiusA + radiusB) / glm::length(pB - pA))) {}

bool VelocityObstacle::isInVO(const glm::vec2 &newVelA) const {
  return glm::dot(glm::normalize(m_posA + newVelA - m_origin), m_ray) >
         glm::cos(m_halfConeAngle);
}

bool VelocityObstacle::isInRVO(const glm::vec2 &newVelA) const {
  return isInVO(newVelA * 2.f - m_velA);
}
