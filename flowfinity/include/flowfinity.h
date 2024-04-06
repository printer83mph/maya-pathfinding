#pragma once

#include "graph.h"
#include "obstacle.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#undef GLM_DISABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec2.hpp>

#include <array>
#include <unordered_map>
#include <vector>

/**
 * Class representing a crowd simulation instance
 */
class FlowFinity {
public:
  FlowFinity();
  ~FlowFinity();

  int size() const;
  const std::vector<glm::vec2> &getAgentPositions() const;
  const std::vector<glm::vec2> &getAgentVelocities() const;
  const std::vector<glm::vec2> &getAgentTargets() const;

  void performTimeStep(float dt);

  void addAgent(const glm::vec2 &pos, const glm::vec2 &target);
  void removeAgent(int index);
  void setAgentTarget(int index, const glm::vec2 &target);

  float getEdgeWeight(glm::vec2 point1, glm::vec2 point2);
  void createGraph(const std::vector<Obstacle> &obstacles);
  void
  addEndPoints(const std::vector<std::pair<glm::vec3, glm::vec3>> &endPoints,
               const std::vector<Obstacle> &obstacles);
  const std::vector<std::pair<glm::vec2, glm::vec2>> &getEdges() const;

  void clearEndPoints();

  void getDisjkstraPaths(std::vector<std::vector<glm::vec3>> &);

  void drawPoints() const;
  void drawVelocities() const;

private:
  struct Config {
    float maxSpeed = 1.f;
    float acceleration = 20.f;
    float drag = 12.f;
    float radius = 0.25f;
    float aggressiveness = 25.f;
  } m_config;

  std::vector<glm::vec2> m_rvoPos;
  std::vector<glm::vec2> m_rvoVel;
  std::vector<glm::vec2> m_rvoTarget;

  std::array<glm::vec2, 33> m_possibleAccels;

  glm::vec2 findOptimalAcceleration(int index, float dt) const;

  // Graph logic
  int m_nextVertex;
  Graph m_graph;
  std::unordered_map<glm::vec3, int, std::hash<glm::vec3>> m_NodeToPoint;
  std::unordered_map<int, glm::vec3> m_PointToNode;
  std::vector<std::pair<glm::vec2, glm::vec2>> edges;
  std::vector<int> m_waypoints;
  std::vector<std::pair<int, int>> m_endPoints;

  void addPoint(const glm::vec3 &point);

  void addAgent(float x, float y);
  void addAgent(const glm::vec2 &pos);
  void removeAgent(size_t index);
};
