#include <obstacle.h>

Obstacle::Obstacle()
    : bounds(std::vector<Edge>()), boundsCount(0),
      boundingBox(glm::vec4(INT_MAX, INT_MAX, INT_MIN, INT_MIN)) {}

Obstacle::Obstacle(const std::vector<Edge> &boundsInput)
    : bounds(boundsInput), boundsCount(boundsInput.size()),
      boundingBox(glm::vec4(INT_MAX, INT_MAX, INT_MIN, INT_MIN)) {

  for (const Edge &edge : bounds) {
    boundingBox.x = std::min(boundingBox.x, edge.point1.x);
    boundingBox.y = std::min(boundingBox.y, edge.point1.z);
    boundingBox.z = std::max(boundingBox.z, edge.point1.x);
    boundingBox.w = std::max(boundingBox.w, edge.point1.z);
  }
}

const std::vector<Edge> &Obstacle::getBounds() const { return bounds; }

void Obstacle::addBound(const Edge &bound) {
  bounds.push_back(bound);
  boundsCount++;
  boundingBox.x = std::min(boundingBox.x, bound.point1.x);
  boundingBox.y = std::min(boundingBox.y, bound.point1.z);
  boundingBox.z = std::max(boundingBox.z, bound.point1.x);
  boundingBox.w = std::max(boundingBox.w, bound.point1.z);
}

const int Obstacle::getBoundsCount() const { return boundsCount; }

int cross(glm::vec2 a, glm::vec2 b) { return a.x * b.y - a.y * b.x; }

int orient(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
  return cross(b - a, c - a);
}

const glm::vec4 Obstacle::getBoundingBox() const { return boundingBox; }

float crossProduct2D(const glm::vec3 &a, const glm::vec3 &b) {
  return a.x * b.z - a.z * b.x;
}

bool onSegment(const glm::vec3 &p, const glm::vec3 &q, const glm::vec3 &r) {
  return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
         q.z <= std::max(p.z, r.z) && q.z >= std::min(p.z, r.z);
}

bool Obstacle::intersects(const Edge edge1, const Edge edge2) {

  // if the edges share any endpoints, return false
  if (edge1.point1 == edge2.point1 || edge1.point1 == edge2.point2 ||
      edge1.point2 == edge2.point1 || edge1.point2 == edge2.point2) {
    return false;
  }

  glm::vec3 p1 = edge1.point1;
  glm::vec3 q1 = edge1.point2;
  glm::vec3 p2 = edge2.point1;
  glm::vec3 q2 = edge2.point2;

  glm::vec3 p1q1 = q1 - p1;
  glm::vec3 p1p2 = p2 - p1;
  glm::vec3 p1q2 = q2 - p1;
  glm::vec3 p2q2 = q2 - p2;
  glm::vec3 p2p1 = p1 - p2;
  glm::vec3 p2q1 = q1 - p2;

  float d1 = crossProduct2D(p1q1, p1p2);
  float d2 = crossProduct2D(p1q1, p1q2);
  float d3 = crossProduct2D(p2q2, p2p1);
  float d4 = crossProduct2D(p2q2, p2q1);

  if (d1 * d2 < 0.0f && d3 * d4 < 0.0f)
    return true;

  if (d1 == 0.0f && onSegment(p1, p2, q1))
    return true;
  if (d2 == 0.0f && onSegment(p1, q2, q1))
    return true;
  if (d3 == 0.0f && onSegment(p2, p1, q2))
    return true;
  if (d4 == 0.0f && onSegment(p2, q1, q2))
    return true;

  return false;
}

bool isCollinear(const glm::vec3 &endPoint1, const glm::vec3 &endPoint2,
                 const glm::vec3 &testPoint) {

  // If the test point is either of the endpoints, then return false to prevent
  // no lines being generated
  if (testPoint == endPoint1 || testPoint == endPoint2) {
    return false;
  }

  float m = (endPoint2.z - endPoint1.z) / (endPoint2.x - endPoint1.x);
  float b = endPoint1.z - m * endPoint1.x;
  return testPoint.z == m * testPoint.x + b;
}

bool Obstacle::isVisible(const glm::vec3 &externalPoint,
                         const glm::vec3 &obstaclePoint,
                         const std::vector<Obstacle> &obstacleList) const {

  // Create bounding box from the direction (obstacles potentially in the way)
  // For each obstacle in the obstacleList, check if the bounding box of the
  // obstacle intersects with the rayBoundingBox
  for (const Obstacle &obstacle : obstacleList) {
    // TODO: Check if ray bouding box intersects with obstacle bounding box
    // (does the obstacle lie in the way?) for optimization
    for (const Edge &edge : obstacle.bounds) {
      // Check if the ray intersects with the edge
      // If the ray intersects with the edge, the obstacle is not visible
      // Or if any of the edge points are collinear with the ray, then return
      // false
      if (edge.point1 != obstaclePoint && edge.point2 != obstaclePoint) {
        if (intersects(Edge{externalPoint, obstaclePoint}, edge)
            //||isCollinear(externalPoint, obstaclePoint, edge.point1)
        ) {
          return false;
        }
      }
    }
  }

  return true;
}

bool Obstacle::isVisibleExternal(const glm::vec3 &externalPoint,
                                 const glm::vec3 &obstaclePoint,
                                 const std::vector<Obstacle> &obstacleList) {
  // For each obstacle in the obstacleList
  for (const Obstacle &obstacle : obstacleList) {
    // TODO: Check if ray bouding box intersects with obstacle bounding box
    // (does the obstacle lie in the way?) for optimization
    for (const Edge &edge : obstacle.bounds) {
      // Check if the ray intersects with the edge
      // If the ray intersects with the edge, the obstacle is not visible
      if (edge.point1 != obstaclePoint && edge.point2 != obstaclePoint) {
        if (intersects(Edge{externalPoint, obstaclePoint}, edge) ||
            isCollinear(externalPoint, obstaclePoint, edge.point1)) {
          return false;
        }
      }
    }
  }

  return true;
}
