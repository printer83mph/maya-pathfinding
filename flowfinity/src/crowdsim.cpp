#include "flowfinity/crowdsim.h"

#include "flowfinity/navigation/navmethod.h"
#include "flowfinity/rvo.h"

#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"

#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <iterator>
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

// // Helper Line Intersection functions for obstacle detection

// // Function to check if two doubles are nearly equal to avoid floating-point issues
// bool almostEqual(double a, double b, double epsilon = 1e-6) { return std::fabs(a - b) < epsilon;
// }

// // Function to check if a point lies on an edge
// bool isPointOnEdge(const glm::vec2& p, const Edge& edge)
// {
//   double minX = std::min(edge.point1.x, edge.point2.x);
//   double maxX = std::max(edge.point1.x, edge.point2.x);
//   double minY = std::min(edge.point1.y, edge.point2.y);
//   double maxY = std::max(edge.point1.y, edge.point2.y);
//   return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY);
// }

// // Function to check if a line in the form of y = mx + b intersects with an edge
// bool doesLineIntersectEdge(double m, double b, const Edge& edge)
// {
//   // Calculate the slope and intercept of the edge
//   double dx = edge.point2.x - edge.point1.x;
//   double dy = edge.point2.y - edge.point1.y;
//   double edgeM = dy / dx;

//   glm::vec2 intersection;

//   // If both lines are vertical or both have the same slope, they are parallel
//   if (almostEqual(dx, 0)) {
//     // Edge is vertical
//     intersection.x = edge.point1.x;
//     intersection.y = m * intersection.x + b;
//   } else if (almostEqual(m, edgeM)) {
//     return false; // Parallel lines do not intersect
//   } else {
//     // Calculate the intersection point between y = mx + b and edge's line equation
//     double edgeB = edge.point1.y - edgeM * edge.point1.x;
//     intersection.x = (edgeB - b) / (m - edgeM);
//     intersection.y = m * intersection.x + b;
//   }

//   // Check if the intersection point lies on the edge
//   return isPointOnEdge(intersection, edge);
// }

void CrowdSim::importAgents(const std::vector<glm::vec2>& pos, const std::vector<glm::vec2>& vel,
                            const std::vector<glm::vec2>& currentTarget,
                            const std::vector<glm::vec2>& finalTarget)
{
  m_rvoPos = pos;
  m_rvoVel = vel;
  m_rvoCurrentTarget = currentTarget;
  m_rvoFinalTarget = finalTarget;
}

void CrowdSim::importAgents(const std::vector<glm::vec2>& pos, const std::vector<glm::vec2>& vel)
{
  m_rvoPos = pos;
  m_rvoVel = vel;

  // fill with empty data (awesome)
  for (int i = 0; i < pos.size(); ++i) {
    m_rvoCurrentTarget.push_back(glm::vec2(0, 0));
    m_rvoFinalTarget.push_back(glm::vec2(0, 0));
  }
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

// if colliding, bump out
#if 1
    for (int j = 0; j < m_rvoPos.size(); ++j) {
      if (i == j)
        continue;
      float dist = glm::length(m_rvoPos[i] - m_rvoPos[j]);
      glm::vec2 dirFromOther = glm::normalize(m_rvoPos[i] - m_rvoPos[j]);
      if (dist < m_config.radius * 2.f) {
        m_rvoPos[i] += dirFromOther * (m_config.radius * 2.f - dist);

        // Special case: if the agent is very close to their final target, then set velocity to 0 to
        // prevent blocking
        if (glm::distance(m_rvoPos[i], m_rvoFinalTarget[i]) < m_config.radius * 2.5f) {
          m_rvoVel[i] = glm::vec2(0, 0);
        }
      }
    }
#endif
  }
}

void CrowdSim::performTimeStep(float dt, NavMethod* navMethod)
{
  // spawn agents
#ifndef BETA
  if (m_config.inOutFlows.size() > 0 && size() < m_config.maxAgents) {
    if ((float)std::rand() / (float)RAND_MAX < dt * m_config.spawnRate) {
      // pick random inOutFlow pair
      int inOutFlowIdx =
          glm::floor((float)std::rand() / (float)RAND_MAX * m_config.inOutFlows.size());
      auto& inOutFlow = m_config.inOutFlows.at(inOutFlowIdx);
      addAgent(inOutFlow.first, inOutFlow.second);
    }
  }
#endif

  // loop through backwards in case we remove agents
  for (int i = size() - 1; i >= 0; --i) {
    auto pos = m_rvoPos.at(i);
    auto currentTarget = m_rvoCurrentTarget.at(i);
    auto finalTarget = m_rvoFinalTarget.at(i);

    // Special case, if not moving but close to final target, then despawn to prevent blocking
    if (glm::length(m_rvoVel[i]) < 0.2f &&
        glm::distance(pos, finalTarget) < m_config.radius * 10.5f) {
#ifndef BETA
      removeAgent(i);
#endif
      continue;
    }

    if (glm::distance(pos, currentTarget) < 0.1f) {
      // Check if current target is final target (we've reached the end)
      if (glm::distance(currentTarget, finalTarget) < 0.45f) {
#ifndef BETA
        removeAgent(i);
#endif
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

  // Bump out from walls, only supported for visibilty graph navmethod
  // Check if navmethod is visibility graph using dynamic cast
  if (dynamic_cast<VisibilityGraph*>(navMethod)) {
    auto vg = dynamic_cast<VisibilityGraph*>(navMethod);
    for (int i = 0; i < m_rvoPos.size(); ++i) {
      auto& pos = m_rvoPos[i];
      auto& vel = m_rvoVel[i];
      auto& target = m_rvoCurrentTarget[i];
      auto& finalTarget = m_rvoFinalTarget[i];

      // For each obstacle, check if the agent is within bounding box
      for (auto& obstacle : vg->getObstacles()) {
        auto& boundingBox = obstacle.getBoundingBox();
        if (pos.x > boundingBox.x && pos.x < boundingBox.z && pos.y > boundingBox.y &&
            pos.y < boundingBox.w) {
          // Find obstacle center by taking median of bounding box

          glm::vec2 obstacleCenter =
              glm::vec2((boundingBox.x + boundingBox.z) / 2, (boundingBox.y + boundingBox.w) / 2);

          // Ray cast to the center of the obstacle, count how many times we hit an edge
          int hitCount = 0;
          // Create an edge from the agent to the obstacle center, scaled to be longer by a factor
          // of 9999
          glm::vec2 edgeDir = glm::normalize(obstacleCenter - pos);
          glm::vec2 edgeEnd = pos + edgeDir * 9999.f;
          Edge m = {glm::vec3(pos.x, 0, pos.y), glm::vec3(edgeEnd.x, 0, edgeEnd.y)};
          for (auto& edge : obstacle.getBounds()) {
            if (Obstacle::intersects(m, edge)) {
              ++hitCount;
            }
          }

          // If we hit an odd number of edges, we are inside the obstacle
          if (hitCount % 2 == 1) {
            // Bump out
            glm::vec2 dirFromObstacle = glm::normalize(pos - obstacleCenter);
            // pos += dirFromObstacle * 0.1f;
            //  Apply a velocity in the bump direction
            vel += dirFromObstacle * 1.1f;
          }
        }
      }
    }
  }
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
  while (currentTargetIndex <= path.size() - 1 &&
         glm ::distance(pos, path.at(currentTargetIndex)) < 0.15) {
    ++currentTargetIndex;
  }
  currentTargetIndex = glm::min(currentTargetIndex, (int)path.size() - 1);
  m_rvoCurrentTarget[index] = path.at(currentTargetIndex);
}

// #define BETA
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
#ifndef BETA
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
#endif

  return *bestAcceleration;
}
