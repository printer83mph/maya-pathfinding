#include "flowfinity/crowdsim.h"

#include "flowfinity/navigation/navmethod.h"
#include "flowfinity/rvo.h"

#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"

#include <iterator>
#include <limits>
#include <vector>

CrowdSim::CrowdSim()
    : m_config(), m_rvoPos(), m_rvoVel(), m_rvoCurrentTarget(), m_rvoFinalTarget(),
      m_possibleAccels()
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

void CrowdSim::importAgents(const std::vector<glm::vec2>& pos, const std::vector<glm::vec2>& vel,
                            const std::vector<glm::vec2>& currentTarget,
                            const std::vector<glm::vec2>& finalTarget)
{
  m_rvoPos = pos;
  m_rvoVel = vel;
  m_rvoCurrentTarget = currentTarget;
  m_rvoFinalTarget = finalTarget;
}

void CrowdSim::unfastComputeAllTargetsFromFirstInOutFlow(NavMethod* navMethod)
{
  for (int i = size() - 1; i >= 0; --i) {
    m_rvoFinalTarget[i] = m_config.inOutFlows.at(0).second;
    computeCurrentTarget(i, navMethod);
  }
}

const std::vector<glm::vec2>& CrowdSim::getAgentPositions() const { return m_rvoPos; }
const std::vector<glm::vec2>& CrowdSim::getAgentVelocities() const { return m_rvoVel; }
const std::vector<glm::vec2>& CrowdSim::getAgentCurrentTargets() const
{
  return m_rvoCurrentTarget;
}
const std::vector<glm::vec2>& CrowdSim::getAgentFinalTargets() const { return m_rvoFinalTarget; }

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

void CrowdSim::performTimeStep(float dt, NavMethod* navMethod)
{
  // spawn agents
  if (m_config.inOutFlows.size() > 0 && size() < m_config.maxAgents) {
    if ((float)std::rand() / (float)RAND_MAX < dt * 0.5f) {
      // pick random inOutFlow pair
      int inOutFlowIdx =
          glm::floor((float)std::rand() / (float)RAND_MAX * m_config.inOutFlows.size());
      auto& inOutFlow = m_config.inOutFlows.at(inOutFlowIdx);
      addAgent(inOutFlow.first, inOutFlow.second);
    }
  }

  // loop through backwards in case we remove agents
  for (int i = size() - 1; i >= 0; --i) {
    auto pos = m_rvoPos.at(i);
    auto currentTarget = m_rvoCurrentTarget.at(i);
    auto finalTarget = m_rvoFinalTarget.at(i);

    if (glm::distance(pos, currentTarget) < 0.05f) {
      // Check if current target is final target (we've reached the end)
      if (glm::distance(currentTarget, finalTarget) < 0.01f) {
        removeAgent(i);
        continue;
      } else {
        // reached our current target - recompute
        computeCurrentTarget(i, navMethod);
        continue;
      }
    }

    // based on some chance, recompute current target
    if ((float)std::rand() / (float)RAND_MAX < dt * 1.f) {
      computeCurrentTarget(i, navMethod);
    }
  }

  // run physics
  performTimeStep(dt);
}

void CrowdSim::addAgent(const glm::vec2& pos, const glm::vec2& target)
{
  m_rvoPos.push_back(pos);
  m_rvoVel.push_back(glm::vec2(0, 0));
  m_rvoCurrentTarget.push_back(target);
  m_rvoFinalTarget.push_back(target);
}

void CrowdSim::removeAgent(int index)
{
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoPos.erase(std::next(m_rvoPos.begin(), index));
  m_rvoVel.erase(std::next(m_rvoVel.begin(), index));
  m_rvoCurrentTarget.erase(std::next(m_rvoCurrentTarget.begin(), index));
  m_rvoFinalTarget.erase(std::next(m_rvoFinalTarget.begin(), index));
}

void CrowdSim::setAgentCurrentTarget(int index, const glm::vec2& target)
{
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoCurrentTarget[index] = target;
}

void CrowdSim::setAgentFinalTarget(int index, const glm::vec2& target)
{
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoFinalTarget[index] = target;
}

void CrowdSim::computeCurrentTarget(int index, NavMethod* navMethod)
{
  auto pos = m_rvoPos.at(index);
  auto path = navMethod->getPath(pos, m_rvoFinalTarget.at(index));

  int currentTargetIndex = 0;
  while (glm::distance(pos, path.at(currentTargetIndex)) < 0.05 &&
         currentTargetIndex <= path.size() - 1) {
    ++currentTargetIndex;
  }

  m_rvoCurrentTarget[index] = path.at(currentTargetIndex);
}

glm::vec2 CrowdSim::findOptimalAcceleration(int index, float dt) const
{
  auto& posA = m_rvoPos[index];
  auto& velA = m_rvoVel[index];
  auto& targetA = m_rvoCurrentTarget[index];

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
    auto& targetB = m_rvoCurrentTarget[i];

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
