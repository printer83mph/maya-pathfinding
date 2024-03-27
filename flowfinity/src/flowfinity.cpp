#include "flowfinity.h"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include "rvo.h"

#include <iterator>
#include <limits>

FlowFinity::FlowFinity()
    : m_rvoPos(), m_rvoVel(), m_rvoTarget(), m_config(), m_possibleAccels() {
  float angle = 0.f;
  for (int i = 0; i < 8; ++i) {
    m_possibleAccels[i].x = glm::cos(angle) + m_config.acceleration;
    m_possibleAccels[i].y = glm::sin(angle) + m_config.acceleration;
    angle += 45.f;
  }
}

FlowFinity::~FlowFinity() {}

int FlowFinity::size() const { return m_rvoPos.size(); }

void FlowFinity::performTimeStep(float dt) {
  for (int i = 0; i < m_rvoPos.size(); ++i) {
    auto accel = findOptimalAcceleration(i, dt);

    // euler integration
    m_rvoVel[i] += accel * dt;
    m_rvoPos[i] += m_rvoVel[i] * dt;
  }
}

const std::vector<glm::vec2> &FlowFinity::getAgentPositions() const {
  return m_rvoPos;
}

void FlowFinity::addAgent(const glm::vec2 &pos, glm::vec2 &target) {
  m_rvoPos.push_back(pos);
  m_rvoVel.push_back(glm::vec2(0, 0));
  m_rvoTarget.push_back(target);
}

void FlowFinity::removeAgent(int index) {
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoPos.erase(std::next(m_rvoPos.begin(), index));
  m_rvoVel.erase(std::next(m_rvoVel.begin(), index));
  m_rvoTarget.erase(std::next(m_rvoTarget.begin(), index));
}

glm::vec2 FlowFinity::findOptimalAcceleration(int index, float dt) const {
  auto &posA = m_rvoPos[index];
  auto &velA = m_rvoVel[index];
  auto &targetA = m_rvoTarget[index];

  glm::vec2 idealAccel = glm::normalize(targetA - posA) * m_config.acceleration;

  const glm::vec2 *bestAcceleration = &m_possibleAccels[0];
  float bestPenalty = std::numeric_limits<float>::max();

  for (int i = 0; i < m_rvoPos.size(); ++i) {
    auto &posB = m_rvoPos[i];
    auto &velB = m_rvoVel[i];
    auto &targetB = m_rvoTarget[i];

    VelocityObstacle vo = VelocityObstacle(posA, velA, posB, velB,
                                           m_config.radius, m_config.radius);

    for (auto &proposedAccel : m_possibleAccels) {
      float penalty = (idealAccel * dt - proposedAccel * dt).length();
      if (vo.isInRVO(velA + proposedAccel * dt))
        // TODO: correct time to collision calculation
        penalty += m_config.aggressiveness / (posA - posB).length();
      if (penalty < bestPenalty) {
        bestPenalty = penalty;
        bestAcceleration = &proposedAccel;
      }
    }
  }

  return glm::vec2(*bestAcceleration);
}
