#include "rvo.h"

#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

VelocityObstacle::VelocityObstacle(const glm::vec2 &pA, const glm::vec2 &pB,
                                   const glm::vec2 &vB, float radiusA,
                                   float radiusB)
    : m_posA(pA), m_origin(pA + vB), m_ray(glm::normalize(pB - pA)),
      m_halfConeAngle(glm::asin((radiusA + radiusB) / glm::length(pB - pA))) {}

bool VelocityObstacle::isInVO(const glm::vec2 &velocityA) const {
  return glm::dot(glm::normalize(m_posA + velocityA - m_origin), m_ray) >
         glm::cos(m_halfConeAngle);
}

ReciprovalVelocityObject::ReciprovalVelocityObject(const glm::vec2 &pA,
                                                   const glm::vec2 &vA,
                                                   const glm::vec2 &pB,
                                                   const glm::vec2 &vB,
                                                   float radiusA, float radiusB)
    : m_aVO(pA, pB, vB, radiusA, radiusB), m_bVO(pB, pA, vA, radiusB, radiusA) {
}

bool ReciprovalVelocityObject::isInRVO(const glm::vec2 velocityA) const {
  // TODO
  return false;
}
