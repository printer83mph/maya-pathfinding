#pragma once
#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>
#include <vector>

struct Edge {
  glm::vec3 point1;
  glm::vec3 point2;
};

class Obstacle
{
public:
  Obstacle();
  Obstacle(const std::vector<Edge>& boundsInput);

  const std::vector<Edge>& getBounds() const;
  void addBound(const Edge& bound);

  // Given an external point and a point on the obstacle, as well as an array of
  // all obstacles returns true if the obstacle point is visible from the
  // external point
  bool isVisible(const glm::vec3& externalPoint, const glm::vec3& obstaclePoint,
                 const std::vector<Obstacle>&) const;

  static bool isVisibleExternal(const glm::vec3& start, const glm::vec3& end,
                                const std::vector<Obstacle>& obstacleList);

  static bool intersects(const Edge edge1, const Edge edge2);
  const int getBoundsCount() const;
  const glm::vec4 getBoundingBox() const;

private:
  std::vector<Edge> bounds;
  // A bounding box containing the obstacle for quick collision detection.
  // The data is stored as (minX, minY, maxX, maxY).
  glm::vec4 boundingBox;
  int boundsCount;
};