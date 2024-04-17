#include "flowfinity/rvo.h"

#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

VelocityObstacle::VelocityObstacle(const glm::vec2& pA, const glm::vec2& vA, const glm::vec2& pB,
                                   const glm::vec2& vB, float radiusA, float radiusB)
    : m_posA(pA), m_velA(vA), m_rayOrigin(pA + vB), m_rayDir(glm::normalize(pB - pA)),
      m_halfConeAngle(
          // TODO: figure out why we have to double this
          glm::asin((radiusA + radiusB) * 2 / glm::length(pB - pA)))
{
}

bool VelocityObstacle::isInVO(const glm::vec2& newVelA) const
{
  return glm::dot(glm::normalize(m_posA + newVelA - m_rayOrigin), m_rayDir) >
         glm::cos(m_halfConeAngle);
}

bool VelocityObstacle::isInRVO(const glm::vec2& newVelA) const
{
  return isInVO(newVelA * 2.f - m_velA);
}
