#include "flowfinity.h"
#include "rvo.h"

#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include <GL/glew.h>

#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

FlowFinity::FlowFinity()
    : m_config(), m_rvoPos(), m_rvoVel(), m_rvoTarget(), m_possibleAccels(),
      m_nextVertex(-1), m_graph(0), m_NodeToPoint(), m_PointToNode(), edges() {
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

FlowFinity::~FlowFinity() {}

int FlowFinity::size() const { return m_rvoPos.size(); }

const std::vector<glm::vec2> &FlowFinity::getAgentPositions() const {
  return m_rvoPos;
}
const std::vector<glm::vec2> &FlowFinity::getAgentVelocities() const {
  return m_rvoVel;
}
const std::vector<glm::vec2> &FlowFinity::getAgentTargets() const {
  return m_rvoTarget;
}

void FlowFinity::performTimeStep(float dt) {
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

void FlowFinity::addAgent(const glm::vec2 &pos, const glm::vec2 &target) {
  m_rvoPos.push_back(pos);
  m_rvoVel.push_back(glm::vec2(0, 0));
  m_rvoTarget.push_back(target);
}

void FlowFinity::removeAgent(int index) {
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoPos.erase(std::next(m_rvoPos.begin(), index));
  m_rvoVel.erase(std::next(m_rvoVel.begin(), index));
  m_rvoTarget.erase(std::next(m_rvoTarget.begin(), index));
}

void FlowFinity::setAgentTarget(int index, const glm::vec2 &target) {
  if (index >= m_rvoPos.size()) {
    throw "index out of bounds lol";
  }
  m_rvoTarget[index] = target;
}

glm::vec2 FlowFinity::findOptimalAcceleration(int index, float dt) const {
  auto &posA = m_rvoPos[index];
  auto &velA = m_rvoVel[index];
  auto &targetA = m_rvoTarget[index];

  glm::vec2 idealAccel = (targetA - posA) * m_config.acceleration;
  {
    float accelLength = glm::length(idealAccel);
    if (accelLength > 4.f) {
      idealAccel *= m_config.acceleration / accelLength;
    }
  }
  const glm::vec2 *bestAcceleration = &idealAccel;
  float bestPenalty = std::numeric_limits<float>::max();

  for (int i = 0; i < m_rvoPos.size(); ++i) {
    if (i == index)
      continue;

    auto &posB = m_rvoPos[i];
    auto &velB = m_rvoVel[i];
    auto &targetB = m_rvoTarget[i];

    VelocityObstacle vo = VelocityObstacle(posA, velA, posB, velB,
                                           m_config.radius, m_config.radius);

    for (auto &proposedAccel : m_possibleAccels) {
      float penalty = glm::length(idealAccel * dt - proposedAccel * dt);
      if (vo.isInRVO(velA + proposedAccel * dt)) {
        // TODO: correct time to collision calculation using evil minowski sum
        float timeToCollision =
            glm::length((posB - posA) / (2.f * proposedAccel - velA - velB));
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

const std::vector<std::pair<glm::vec2, glm::vec2>> &
FlowFinity::getEdges() const {
  return edges;
}

// Add endpoints to the graph and create edges between them and the obstacles
void FlowFinity::addEndPoints(
    const std::vector<std::pair<glm::vec3, glm::vec3>> &endPoints,
    const std::vector<Obstacle> &obstacles) {
  // TODO
  int totalNodes = 0;

  // For each pair of endpoints
  for (auto &pair : endPoints) {
    glm::vec3 start = pair.first;
    glm::vec3 end = pair.second;

    // Add endpoints to the maps
    if (m_NodeToPoint.find(start) == m_NodeToPoint.end()) {
      m_nextVertex++;
      m_NodeToPoint[start] = m_nextVertex;
      m_PointToNode[m_nextVertex] = start;
      totalNodes++;
    }
    if (m_NodeToPoint.find(end) == m_NodeToPoint.end()) {
      m_nextVertex++;
      m_NodeToPoint[end] = m_nextVertex;
      m_PointToNode[m_nextVertex] = end;
      totalNodes++;
    }

    // With the total unique endpoints, add them to the map
    m_graph.vertices = m_graph.vertices + totalNodes;
    // Add the new rows
    for (int i = 0; i < totalNodes; i++) {
      m_graph.adjMatrix.push_back(std::vector<float>());
      m_graph.adjMatrix.back().resize(m_graph.vertices);
    }
    // Update the old rows
    for (int i = 0; i < m_graph.vertices - totalNodes; i++) {
      m_graph.adjMatrix[i].resize(m_graph.vertices);
    }

    // Create an edge between the two endpoints if they are visible
    if (Obstacle::isVisibleExternal(start, end, obstacles)) {
      m_graph.addEdge(m_NodeToPoint[start], m_NodeToPoint[end],
                      glm::distance(start, end));
      edges.push_back(std::pair<glm::vec2, glm::vec2>(
          glm::vec2(start.x, start.z), glm::vec2(end.x, end.z)));
    }

    // For each obstacle, check if the edge between the start and end point to
    // each obstacle point is visible, if so create an edge
    for (auto &obstacle : obstacles) {
      // TODO: Optimize
      for (auto &edge : obstacle.getBounds()) {
        if (obstacle.isVisible(start, edge.point1, obstacles)) {
          m_graph.addEdge(m_NodeToPoint[start], m_NodeToPoint[edge.point1],
                          glm::distance(start, edge.point1));
          edges.push_back(std::pair<glm::vec2, glm::vec2>(
              glm::vec2(start.x, start.z),
              glm::vec2(edge.point1.x, edge.point1.z)));
        }
        if (obstacle.isVisible(end, edge.point1, obstacles)) {
          m_graph.addEdge(m_NodeToPoint[end], m_NodeToPoint[edge.point1],
                          glm::distance(end, edge.point1));
          edges.push_back(std::pair<glm::vec2, glm::vec2>(
              glm::vec2(end.x, end.z),
              glm::vec2(edge.point1.x, edge.point1.z)));
        }
      }
    }
  }
}

// Just create a graph with the given obstacles
void FlowFinity::createGraph(const std::vector<Obstacle> &obstacles) {
  // Make sure to clear the graph and maps
  m_NodeToPoint.clear();
  m_PointToNode.clear();
  edges.clear();
  m_nextVertex = -1;

  // Get total amount of vertices
  int totalNodes = 0;
  // Record total amount of waypoints
  for (auto &obstacle : obstacles) {
    totalNodes += obstacle.getBoundsCount();
  }
  m_graph = Graph(totalNodes);

  // For each obstacle
  for (auto &obstacle : obstacles) {
    // For each edge in the obstacle
    for (auto &edge : obstacle.getBounds()) {
      // For each other obstacle
      for (auto &obstacle2 : obstacles) {
        // For each edge in the other obstacle
        if (&obstacle == &obstacle2) {
          continue;
        }
        for (auto &edge2 : obstacle2.getBounds()) {
          // If the two edges are visible to each other, add an edge to the map
          if (obstacle.isVisible(edge2.point1, edge.point1, obstacles)) {
            // If either of the points are not in the map, add them
            bool point1Exists =
                m_NodeToPoint.find(edge.point1) != m_NodeToPoint.end();
            bool point2Exists =
                m_NodeToPoint.find(edge2.point1) != m_NodeToPoint.end();

            if (!point1Exists) {
              m_nextVertex++;
              m_NodeToPoint[edge.point1] = m_nextVertex;
              m_PointToNode[m_nextVertex] = edge.point1;
            }
            if (!point2Exists) {
              m_nextVertex++;
              m_NodeToPoint[edge2.point1] = m_nextVertex;
              m_PointToNode[m_nextVertex] = edge2.point1;
            }
            // Add the edge to the graph if the edges are not the same and the
            // edge doesn't already exist
            if (m_graph.getEdge(m_NodeToPoint[edge.point1],
                                m_NodeToPoint[edge2.point1]) == 0 &&
                edge.point1 != edge2.point1) {
              m_graph.addEdge(m_NodeToPoint[edge.point1],
                              m_NodeToPoint[edge2.point1],
                              glm::distance(edge.point1, edge2.point1));
              // Add the edge to the list of edges
              edges.push_back(std::pair<glm::vec2, glm::vec2>(
                  glm::vec2(edge.point1.x, edge.point1.z),
                  glm::vec2(edge2.point1.x, edge2.point1.z)));
            }
          }
        }
      }
    }
  }

  // Add adjacent points within the same obstacle to the graph
  for (auto &obstacle : obstacles) {
    for (auto &edge : obstacle.getBounds()) {
      // All obstacle points are guaranteed to be in the map, so just add them
      // using the maps
      m_graph.addEdge(m_NodeToPoint[edge.point1], m_NodeToPoint[edge.point2],
                      glm::distance(edge.point1, edge.point2));
      edges.push_back(std::pair<glm::vec2, glm::vec2>(
          glm::vec2(edge.point1.x, edge.point1.z),
          glm::vec2(edge.point2.x, edge.point2.z)));
    }
  }
}

float FlowFinity::getEdgeWeight(glm::vec2 point1, glm::vec2 point2) {
  return m_graph.getEdge(m_NodeToPoint[glm::vec3(point1.x, 0, point1.y)],
                         m_NodeToPoint[glm::vec3(point2.x, 0, point2.y)]);
}

int minDistance(int dist[], bool sptSet[], int V) {

  // Initialize min value
  int min = INT_MAX, min_index;

  for (int v = 0; v < V; v++)
    if (sptSet[v] == false && dist[v] <= min)
      min = dist[v], min_index = v;

  return min_index;
}

std::vector<glm::vec3> FlowFinity::getDisjkstraPath(glm::vec3 start,
                                                    glm::vec3 end) {
  std::vector<glm::vec3> path;
  int src, dst;
  // Try to find the start and end points in the map, otherwise, go through map
  // to find the closest point
  if (m_NodeToPoint.find(start) != m_NodeToPoint.end()) {
    src = m_NodeToPoint[start];
  } else {
    float minDist = INT_MAX;
    for (auto &point : m_NodeToPoint) {
      float dist = glm::distance(glm::vec2(start.x, start.z),
                                 glm::vec2(point.first.x, point.first.z));
      if (dist < minDist) {
        minDist = dist;
        src = point.second;
      }
    }
  }

  if (m_NodeToPoint.find(end) != m_NodeToPoint.end()) {
    dst = m_NodeToPoint[end];
  } else {
    float minDist = INT_MAX;
    for (auto &point : m_NodeToPoint) {
      float dist = glm::distance(glm::vec2(end.x, end.z),
                                 glm::vec2(point.first.x, point.first.z));
      if (dist < minDist) {
        minDist = dist;
        dst = point.second;
      }
    }
  }

  int nVertices = m_graph.getVertices();

  // shortestDistances[i] will hold the shortest distance from src to i
  std::vector<float> shortestDistances(nVertices);

  // visited[i] will be true if the vertex has been visited by the algorithm
  std::vector<bool> visited(nVertices);

  // Initialize all distances as
  // INFINITE and added[] as false
  for (int vertexIndex = 0; vertexIndex < nVertices; vertexIndex++) {
    shortestDistances[vertexIndex] = INT_MAX;
    visited[vertexIndex] = false;
  }

  // Distance of source vertex from
  // itself is always 0
  shortestDistances[src] = 0;

  // Parent array to store shortest
  // path tree
  std::vector<int> parents(nVertices);

  // The starting vertex does not have a parent
  parents[src] = -1;

  // The source has been visited
  visited[src] = true;

  // Starting from source, go through each vertex
  int curr = src;
  for (int i = 1; i < nVertices; i++) {
    // For each vertex, go through all the edges
    for (int j = 0; j < nVertices; j++) {
      // If the vertex is not visited and the distance is less than the
      // shortest distance
      if (!visited[j] && m_graph.getEdge(curr, j) != 0 &&
          shortestDistances[curr] + m_graph.getEdge(curr, j) <
              shortestDistances[j]) {
        // Update the shortest distance
        shortestDistances[j] =
            shortestDistances[curr] + m_graph.getEdge(curr, j);
        // Update the parent
        parents[j] = curr;
      }
    }

    // Find the vertex with the smallest distance
    int nextVertex = -1;
    float minDistance = INT_MAX;
    for (int j = 0; j < nVertices; j++) {
      if (!visited[j] && shortestDistances[j] < minDistance) {
        nextVertex = j;
        minDistance = shortestDistances[j];
      }
    }

    // If there is no next vertex, break
    if (nextVertex == -1) {
      break;
    }

    // Mark the vertex as visited
    visited[nextVertex] = true;
    curr = nextVertex;
  }

  int i = dst;
  while (i != -1) {
    path.push_back(m_PointToNode[i]);
    i = parents[i];
  }

  std::reverse(path.begin(), path.end());

  // std::cout << src << ", " << dst << std::endl;

  for (auto &i : parents) {
    // std::cout << i << ", " << std::endl;
  }

  for (auto &i : path) {
    std::cout << i.x << ", " << i.y << ", " << i.z << std::endl;
  }

  return path;
}

void FlowFinity::drawPoints() const {
  glBegin(GL_POINTS);
  for (auto &pos : m_rvoPos) {
    glVertex3f(pos.x, 0, pos.y);
  }
  glEnd();
}

void FlowFinity::drawVelocities() const {
  glBegin(GL_LINES);
  for (int i = 0; i < size(); ++i) {
    const auto &pos = m_rvoPos[i];
    const auto &vel = m_rvoVel[i];
    auto end = pos + vel * 0.5f;
    glColor3f(0, 0, 1);
    glVertex3f(pos.x, 0, pos.y);
    glColor3f(0, 0, 1);
    glVertex3f(end.x, 0, end.y);
  }
  glEnd();
}