#include <obstacle.h>

Obstacle::Obstacle() : bounds(std::vector<Edge>()) {}

Obstacle::Obstacle(const std::vector<Edge> &boundsInput)
    : bounds(boundsInput) {}

const std::vector<Edge> &Obstacle::getBounds() const { return bounds; }

void Obstacle::addBound(const Edge &bound) { bounds.push_back(bound); }

bool Obstacle::intersects(const Edge edge1, const Edge edge2) {
  // Calculate the determinant of the matrix formed by the vectors of the edges
  float determinant =
      (edge1.point2.x - edge1.point1.x) * (edge2.point2.z - edge2.point1.z) -
      (edge1.point2.z - edge1.point1.z) * (edge2.point2.x - edge2.point1.x);
  // If the determinant is 0, the edges are parallel and do not intersect
  if (determinant == 0) {
    return false;
  }

  // Calculate the intersection point of the two edges
  float t =
      ((edge1.point1.x - edge2.point1.x) * (edge2.point2.z - edge2.point1.z) -
       (edge1.point1.z - edge2.point1.z) * (edge2.point2.x - edge2.point1.x)) /
      determinant;
  float u =
      -((edge1.point1.x - edge2.point1.x) * (edge1.point2.z - edge1.point1.z) -
        (edge1.point1.z - edge2.point1.z) * (edge1.point2.x - edge1.point1.x)) /
      determinant;

  // If the intersection point is on both edges, the edges intersect
  return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

bool Obstacle::isVisible(const glm::vec3 &externalPoint,
                         const glm::vec3 &obstaclePoint,
                         const std::vector<Obstacle> &obstacleList) const {
  // Create bounding box from the direction (obstacles potentially in the way)
  glm::vec4 rayBoundingBox(std::min(externalPoint.x, obstaclePoint.x),
                           std::min(externalPoint.z, obstaclePoint.z),
                           std::max(externalPoint.x, obstaclePoint.x),
                           std::max(externalPoint.z, obstaclePoint.z));
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
          (obstacle.boundingBox.w < rayBoundingBox.w)))) {
      // If the bounding boxes intersect, check if the ray intersects with the
      // obstacle
      for (const Edge &edge : obstacle.bounds) {
        // Check if the ray intersects with the edge
        // If the ray intersects with the edge, the obstacle is not visible
        if (intersects(Edge{externalPoint, obstaclePoint}, edge)) {
          return false;
        }
      }
    }
  }

  return true;
}
