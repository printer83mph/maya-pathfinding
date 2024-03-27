#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtx/hash.hpp"
#include <glm/vec2.hpp>
#include <graph.h>
#include <obstacle.h>
#include <unordered_map>
#include <vector>

/**
 * Class representing a crowd simulation instance
 */
class FlowFinity {
public:
  FlowFinity();
  ~FlowFinity();

  void performTimeStep(float dt);

  float getEdgeWeight(glm::vec2 point1, glm::vec2 point2);
  void createGraph(const std::vector<Obstacle> &obstacles);
  const std::vector<std::pair<glm::vec2, glm::vec2>> &getEdges() const;

private:
  std::vector<glm::vec2> m_pos;
  std::vector<glm::vec2> m_vel;

  // Graph logic
  int m_nextVertex;
  Graph m_graph;
  std::unordered_map<glm::vec3, int, std::hash<glm::vec3>> m_NodeToPoint;
  std::vector<std::pair<glm::vec2, glm::vec2>> edges;

  void addAgent(float x, float y);
  void addAgent(const glm::vec2 &pos);
  void removeAgent(size_t index);
};