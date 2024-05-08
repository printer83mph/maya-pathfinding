#pragma once

#include "flowfinity/navigation/obstacle.h"
#include "flowfinity/util/graph.h"
#include "navmethod.h"
#include "obstacle.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#undef GLM_DISABLE_EXPERIMENTAL
#include <glm/vec2.hpp>

#include <unordered_map>
#include <vector>

class VisibilityGraph : public NavMethod
{
public:
  VisibilityGraph();
  virtual ~VisibilityGraph();

  void addCubeObstacle(const glm::vec2& translation, const glm::vec2& scale, float rotation,
                       float radius = 0.25f);
  void clearObstacles();
  void clearEndPoints();

  void createGraph();

  // for NavMethod
  std::vector<glm::vec2> getPath(const glm::vec2& start, const glm::vec2& end) override;

  const std::vector<std::pair<glm::vec2, glm::vec2>>& getEdges() const;
  const std::vector<Obstacle>& getObstacles() const;

private:
  // Graph logic
  int m_nextVertex;
  Graph m_graph;
  std::vector<Obstacle> m_obstacles;
  std::unordered_map<glm::vec3, int, std::hash<glm::vec3>> m_NodeToPoint;
  std::unordered_map<int, glm::vec3> m_PointToNode;
  std::vector<std::pair<glm::vec2, glm::vec2>> edges;
  std::vector<int> m_waypoints;
  std::vector<std::pair<int, int>> m_endPoints;
  std::vector<std::vector<glm::vec3>> m_paths;

  bool m_graphCreated;

  float getEdgeWeight(glm::vec2 point1, glm::vec2 point2);
  void addPoint(const glm::vec3& point);

  void addEndPoints(const std::vector<std::pair<glm::vec3, glm::vec3>>& endPoints);

  void getDisjkstraPaths(const std::vector<std::pair<glm::vec3, glm::vec3>>& endpoints,
                         std::vector<std::vector<glm::vec3>>& outPaths);

  static int minDistance(int dist[], bool sptSet[], int V);
};
