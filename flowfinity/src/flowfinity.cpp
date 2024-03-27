#include "flowfinity.h"
#include "glm/geometric.hpp"

#include <iostream>
#include <iterator>

FlowFinity::FlowFinity()
    : m_pos(), m_vel(), m_nextVertex(-1), m_graph(0), m_NodeToPoint(), edges() {
}

FlowFinity::~FlowFinity() {}

void FlowFinity::performTimeStep(float dt) {
  // TODO
}

void FlowFinity::addAgent(float x, float y) {
  m_pos.push_back(glm::vec2(x, y));
  m_vel.push_back(glm::vec2(0, 0));
}

void FlowFinity::addAgent(const glm::vec2 &pos) { addAgent(pos.x, pos.y); }

void FlowFinity::removeAgent(size_t index) {
  if (index >= m_pos.size()) {
    throw "index out of bounds lol";
  }
  m_pos.erase(std::next(m_pos.begin(), index));
  m_vel.erase(std::next(m_vel.begin(), index));
}

const std::vector<std::pair<glm::vec2, glm::vec2>> &
FlowFinity::getEdges() const {
  return edges;
}

// Just create a graph with the given obstacles
void FlowFinity::createGraph(const std::vector<Obstacle> &obstacles) {
  // For each obstacle
  // TODO: change this when we have more than just cubes lol
  m_graph = Graph(obstacles.size() * 4);
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
            }
            if (!point2Exists) {
              m_nextVertex++;
              m_NodeToPoint[edge2.point1] = m_nextVertex;
            }
            // Add the edge to the graph
            if (!(point1Exists && point2Exists) &&
                edge.point1 != edge2.point1) {
              m_graph.addEdge(m_NodeToPoint[edge.point1],
                              m_NodeToPoint[edge2.point1],
                              glm::distance(edge.point1, edge2.point1));
              // std::cout << "Adding edge from " << m_NodeToPoint[edge.point1]
              //           << " at " << edge.point1 << " to "
              //           << m_NodeToPoint[edge2.point1] << " at " <<
              //           edge2.point1
              //           << std::endl;
              edges.push_back(std::pair<glm::vec2, glm::vec2>(
                  glm::vec2(edge.point1.x, edge.point1.z),
                  glm::vec2(edge2.point1.x, edge2.point1.z)));
            }
          }
        }
      }
    }
  }
}