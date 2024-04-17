#pragma once

#include "flowfinity/util/graph.h"
#include "navmethod.h"
#include "obstacle.h"


#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <unordered_map>

class VisibilityGraph : public NavMethod
{
public:
  VisibilityGraph();

  // for NavMethod
  std::vector<glm::vec2> getPath(const glm::vec2& start, const glm::vec2& end) override;

  void addCubeObstacle(const glm::vec2& translation, const glm::vec2& scale, float rotation);
  void clearObstacles();

private:
  // Graph logic
  int m_nextVertex;
  Graph m_graph;
  std::vector<Obstacle> m_obstacles;
  std::unordered_map<int, glm::vec3> m_PointToNode;
  std::vector<std::pair<glm::vec2, glm::vec2>> edges;
  std::vector<int> m_waypoints;
  std::vector<std::pair<int, int>> m_endPoints;

  int getPointFromNode(const glm::vec3& node) const;
  const std::vector<std::pair<glm::vec2, glm::vec2>>& getEdges() const;
  float getEdgeWeight(glm::vec2 point1, glm::vec2 point2) const;
  void addPoint(const glm::vec3& point);

  void createGraph(const std::vector<Obstacle>& obstacles);

  void addEndPoints(const std::vector<std::pair<glm::vec3, glm::vec3>>& endPoints,
                    const std::vector<Obstacle>& obstacles);
  void clearEndPoints();

  void getDisjkstraPaths(std::vector<std::vector<glm::vec3>>&) const;
  std::vector<std::vector<glm::vec3>> m_paths;

  static int minDistance(int dist[], bool sptSet[], int V);
};
