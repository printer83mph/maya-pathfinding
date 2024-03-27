#include <obstacle.h>

Obstacle::Obstacle() : bounds(std::vector<Edge>()) {}

Obstacle::Obstacle(const std::vector<Edge> &boundsInput)
    : bounds(boundsInput) {}

const std::vector<Edge> &Obstacle::getBounds() const { return bounds; }

void Obstacle::addBound(const Edge &bound) { bounds.push_back(bound); }

int cross(glm::vec2 a, glm::vec2 b) { return a.x * b.y - a.y * b.x; }

int orient(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
  return cross(b - a, c - a);
}

bool Obstacle::intersects(const Edge edge1, const Edge edge2) {
  glm::vec2 a = glm::vec2(edge1.point1.x, edge1.point1.z);
  glm::vec2 b = glm::vec2(edge1.point2.x, edge1.point2.z);
  glm::vec2 c = glm::vec2(edge2.point1.x, edge2.point1.z);
  glm::vec2 d = glm::vec2(edge2.point2.x, edge2.point2.z);

  int oa = orient(c, d, a), ob = orient(c, d, b), oc = orient(a, b, c),
      od = orient(a, b, d);
  // Proper intersection exists iff opposite signs
  return (oa * ob < 0 && oc * od < 0);
}

bool Obstacle::isVisible(const glm::vec3 &externalPoint,
                         const glm::vec3 &obstaclePoint,
                         const std::vector<Obstacle> &obstacleList) const {
  // Create bounding box from the direction (obstacles potentially in the way)
  glm::vec4 rayBoundingBox(std::min(externalPoint.x, obstaclePoint.x),
                           std::min(externalPoint.z, obstaclePoint.z),
                           std::max(externalPoint.x, obstaclePoint.x),
                           std::max(externalPoint.z, obstaclePoint.z));

  // Check if the obstacle's own bounds box intersects with the edge from the
  // external to obstacle point
  // for (const Edge &edge : bounds) {
  //   // Check if the edge does not already contain the obstaclePoint
  //   if (edge.point1 != obstaclePoint && edge.point2 != obstaclePoint) {
  //     if (intersects(Edge{externalPoint, obstaclePoint}, edge)) {
  //       return false;
  //     }
  //   }
  // }

  // For each obstacle in the obstacleList, check if the bounding box of the
  // obstacle intersects with the rayBoundingBox
  for (const Obstacle &obstacle : obstacleList) {
    if ((((obstacle.boundingBox.x > rayBoundingBox.x) &&
          (obstacle.boundingBox.x < rayBoundingBox.z)) ||
         (obstacle.boundingBox.z > rayBoundingBox.x) &&
             (obstacle.boundingBox.z < rayBoundingBox.z)) &&
            (((obstacle.boundingBox.y > rayBoundingBox.y) &&
              (obstacle.boundingBox.y < rayBoundingBox.w)) ||
             ((obstacle.boundingBox.w > rayBoundingBox.y) &&
              (obstacle.boundingBox.w < rayBoundingBox.w))) ||
        (obstacle.boundingBox.x == this->boundingBox.x &&
         obstacle.boundingBox.y == this->boundingBox.y &&
         obstacle.boundingBox.z == this->boundingBox.z &&
         obstacle.boundingBox.w == this->boundingBox.w)) {
      // If the bounding boxes intersect, check if the ray intersects with the
      // obstacle
      for (const Edge &edge : obstacle.bounds) {
        // Check if the ray intersects with the edge
        // If the ray intersects with the edge, the obstacle is not visible
        if (edge.point1 != obstaclePoint && edge.point2 != obstaclePoint) {
          if (intersects(Edge{externalPoint, obstaclePoint}, edge)) {
            return false;
          }
        }
      }
    }
  }

  return true;
}
