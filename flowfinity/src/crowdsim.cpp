#include "flowfinity/crowdsim.h"

#include "flowfinity/rvo.h"

#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"

#include <iterator>
#include <limits>
#include <vector>

CrowdSim::CrowdSim() : m_config(), m_rvoPos(), m_rvoVel(), m_rvoTarget(), m_possibleAccels()
{
  float angle = 0.f;
  m_possibleAccels[32] = glm::vec2(0, 0);
  for (int i = 0; i < 16; ++i) {
    angle += glm::two_pi<float>() / 16;
    float cBase = glm::cos(angle) * m_config.acceleration;
    float sBase = glm::sin(angle) * m_config.acceleration;
    m_possibleAccels[i * 2].x = cBase;
    m_possibleAccels[i * 2].y = sBase;
    m_possibleAccels[i * 2 + 1].x = cBase * 0.5f;
    m_possibleAccels[i * 2 + 1].y = sBase * 0.5f;
  }
}

CrowdSim::~CrowdSim() {}

int CrowdSim::size() const { return m_rvoPos.size(); }

const std::vector<glm::vec2>& CrowdSim::getAgentPositions() const { return m_rvoPos; }
const std::vector<glm::vec2>& CrowdSim::getAgentVelocities() const { return m_rvoVel; }
const std::vector<glm::vec2>& CrowdSim::getAgentTargets() const { return m_rvoTarget; }

void CrowdSim::performTimeStep(float dt)
{
  for (int i = 0; i < m_rvoPos.size(); ++i) {
    auto accel = findOptimalAcceleration(i, dt);

    // drag
    m_rvoVel[i] *= (1.f - m_config.drag * dt);

    // euler integration
    m_rvoVel[i] += accel * dt;
    m_rvoPos[i] += m_rvoVel[i] * dt;

#if 0
    // if colliding, bump out
    for (int j = 0; j < m_rvoPos.size(); ++j) {
      if (i == j)
        continue;
      float dist = glm::length(m_rvoPos[i] - m_rvoPos[j]);
      glm::vec2 dirFromOther = glm::normalize(m_rvoPos[i] - m_rvoPos[j]);
      if (dist < m_config.radius * 2.f) {
        m_rvoPos[i] += dirFromOther * (m_config.radius * 2.f - dist);
      }
    }
#endif
  }
}

void CrowdSim::addAgent(const glm::vec2& pos, const glm::vec2& target)
{
  m_rvoPos.push_back(pos);
  m_rvoVel.push_back(glm::vec2(0, 0));
  m_rvoTarget.push_back(target);
}

void CrowdSim::removeAgent(int index)
{
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoPos.erase(std::next(m_rvoPos.begin(), index));
  m_rvoVel.erase(std::next(m_rvoVel.begin(), index));
  m_rvoTarget.erase(std::next(m_rvoTarget.begin(), index));
}

void CrowdSim::setAgentTarget(int index, const glm::vec2& target)
{
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoTarget[index] = target;
}

glm::vec2 CrowdSim::findOptimalAcceleration(int index, float dt) const
{
  auto& posA = m_rvoPos[index];
  auto& velA = m_rvoVel[index];
  auto& targetA = m_rvoTarget[index];

  glm::vec2 idealAccel = (targetA - posA) * m_config.acceleration;
  {
    float accelLength = glm::length(idealAccel);
    if (accelLength > 4.f) {
      idealAccel *= m_config.acceleration / accelLength;
    }
  }
  const glm::vec2* bestAcceleration = &idealAccel;
  float bestPenalty = std::numeric_limits<float>::max();

  for (int i = 0; i < m_rvoPos.size(); ++i) {
    if (i == index)
      continue;

    auto& posB = m_rvoPos[i];
    auto& velB = m_rvoVel[i];
    auto& targetB = m_rvoTarget[i];

    VelocityObstacle vo =
        VelocityObstacle(posA, velA, posB, velB, m_config.radius, m_config.radius);

    for (auto& proposedAccel : m_possibleAccels) {
      float penalty = glm::length(idealAccel * dt - proposedAccel * dt);
      if (vo.isInRVO(velA + proposedAccel * dt)) {
        // TODO: correct time to collision calculation using evil minowski sum
        float timeToCollision = glm::length((posB - posA) / (2.f * proposedAccel - velA - velB));
        penalty += m_config.aggressiveness / timeToCollision;
      }
      if (penalty < bestPenalty) {
        bestPenalty = penalty;
        bestAcceleration = &proposedAccel;
      }
    }
  }

  return *bestAcceleration;
}
