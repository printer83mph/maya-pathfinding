#include "flowfinity/navigation/visibilitygraph.h"

VisibilityGraph::VisibilityGraph()
    : m_nextVertex(-1), m_graph(0), m_NodeToPoint(), m_PointToNode(), edges(), m_waypoints(),
      m_obstacles()
{
}

VisibilityGraph::~VisibilityGraph() {}

std::vector<glm::vec2> VisibilityGraph::getPath(const glm::vec2& start, const glm::vec2& end)
{
  // get path
  clearEndPoints();
  auto paths = std::vector<std::vector<glm::vec3>>();
  addEndPoints({{glm::vec3(start.x, 0, start.y), glm::vec3(end.x, 0, end.y)}}, m_obstacles);
  getDisjkstraPaths(paths);
  auto pathVec3 = paths.at(0);

  // transform to vec2s
  auto pathVec2 = std::vector<glm::vec2>(pathVec3.size());
  for (auto& pos : pathVec3) {
    pathVec2.push_back(glm::vec2(pos.x, pos.z));
  }
  return pathVec2;
}

void VisibilityGraph::addCubeObstacle(const glm::vec2& translation, const glm::vec2& scale,
                                      float rotation)
{
  // Default cube vertices
  std::vector<glm::vec2> pos_data{glm::vec2(-1, -1), glm::vec2(-1, 1), glm::vec2(1, 1),
                                  glm::vec2(1, -1)};

  // Set cube transformations
  glm::mat2 scale_mat = glm::mat2(scale.x, 0, 0, scale.y);
  glm::mat2 rot = glm::mat2(glm::cos(glm::radians(rotation)), -glm::sin(glm::radians(rotation)),
                            glm::sin(glm::radians(rotation)), glm::cos(glm::radians(rotation)));

  // Apply transformations to cube vertices
  for (int i = 0; i < 4; i++) {
    pos_data[i] = (rot * scale_mat * pos_data[i]) + translation;
  }

  // Create obstacle from transformed cube vertices
  Obstacle obstacle = Obstacle();
  obstacle.addBound(Edge{glm::vec3(pos_data[0][0], 0, pos_data[0][1]),
                         glm::vec3(pos_data[1][0], 0, pos_data[1][1])});
  obstacle.addBound(Edge{glm::vec3(pos_data[1][0], 0, pos_data[1][1]),
                         glm::vec3(pos_data[2][0], 0, pos_data[2][1])});
  obstacle.addBound(Edge{glm::vec3(pos_data[2][0], 0, pos_data[2][1]),
                         glm::vec3(pos_data[3][0], 0, pos_data[3][1])});
  obstacle.addBound(Edge{glm::vec3(pos_data[3][0], 0, pos_data[3][1]),
                         glm::vec3(pos_data[0][0], 0, pos_data[0][1])});
  // if the created obstacle is not already in the same obstacle (same bounds),
  // then add it
  for (auto& obstacleInList : m_obstacles) {
    glm::vec4 boundingBox = obstacle.getBoundingBox();
    glm::vec4 boundingBoxInList = obstacleInList.getBoundingBox();
    if (boundingBox.x == boundingBoxInList.x && boundingBox.y == boundingBoxInList.y &&
        boundingBox.z == boundingBoxInList.z && boundingBox.w == boundingBoxInList.w) {
      return;
    }
  }
  m_obstacles.push_back(obstacle);
}

void VisibilityGraph::clearObstacles() { m_obstacles.clear(); }

const std::vector<std::pair<glm::vec2, glm::vec2>>& VisibilityGraph::getEdges() const
{
  return edges;
}

float VisibilityGraph::getEdgeWeight(glm::vec2 point1, glm::vec2 point2)
{
  return m_graph.getEdge(m_NodeToPoint[glm::vec3(point1.x, 0, point1.y)],
                         m_NodeToPoint[glm::vec3(point2.x, 0, point2.y)]);
}

void VisibilityGraph::addPoint(const glm::vec3& point)
{
  if (m_NodeToPoint.find(point) == m_NodeToPoint.end()) {
    m_nextVertex++;
    m_NodeToPoint[point] = m_nextVertex;
    m_PointToNode[m_nextVertex] = point;
  }
}

// Just create a graph with the given obstacles
void VisibilityGraph::createGraph(const std::vector<Obstacle>& obstacles)
{
  // Make sure to clear the graph and maps
  m_NodeToPoint.clear();
  m_PointToNode.clear();
  edges.clear();
  m_nextVertex = -1;

  // Get total amount of vertices
  int totalNodes = 0;
  // Record total amount of waypoints
  for (auto& obstacle : obstacles) {
    totalNodes += obstacle.getBoundsCount();
  }
  m_graph = Graph(totalNodes);

  // For each obstacle
  for (auto& obstacle : obstacles) {
    // For each edge in the obstacle
    for (auto& edge : obstacle.getBounds()) {
      // For each other obstacle
      for (auto& obstacle2 : obstacles) {
        // For each edge in the other obstacle
        if (&obstacle == &obstacle2) {
          continue;
        }
        for (auto& edge2 : obstacle2.getBounds()) {
          // If the two edges are visible to each other, add an edge to the map
          if (obstacle.isVisible(edge2.point1, edge.point1, obstacles)) {
            // If either of the points are not in the map, add them
            addPoint(edge.point1);
            addPoint(edge2.point1);
            addPoint(edge.point2);
            addPoint(edge2.point2);
            // Add the edge to the graph if the edges are not the same and the
            // edge doesn't already exist
            if (m_graph.getEdge(m_NodeToPoint[edge.point1], m_NodeToPoint[edge2.point1]) == 0 &&
                edge.point1 != edge2.point1) {
              m_graph.addEdge(m_NodeToPoint[edge.point1], m_NodeToPoint[edge2.point1],
                              glm::distance(edge.point1, edge2.point1));
              // Add the edge to the list of edges
              edges.push_back(
                  std::pair<glm::vec2, glm::vec2>(glm::vec2(edge.point1.x, edge.point1.z),
                                                  glm::vec2(edge2.point1.x, edge2.point1.z)));
            }
          }
        }
      }
    }
  }

  // Add adjacent points within the same obstacle to the graph
  for (auto& obstacle : obstacles) {
    for (auto& edge : obstacle.getBounds()) {
      // All obstacle points are guaranteed to be in the map, so just add them
      // using the maps
      m_graph.addEdge(m_NodeToPoint[edge.point1], m_NodeToPoint[edge.point2],
                      glm::distance(edge.point1, edge.point2));
      edges.push_back(std::pair<glm::vec2, glm::vec2>(glm::vec2(edge.point1.x, edge.point1.z),
                                                      glm::vec2(edge.point2.x, edge.point2.z)));
    }
  }
}

// Add endpoints to the graph and create edges between them and the obstacles
void VisibilityGraph::addEndPoints(const std::vector<std::pair<glm::vec3, glm::vec3>>& endPoints,
                                   const std::vector<Obstacle>& obstacles)
{
  int totalNodes = 0;

  // For each pair of endpoints
  for (auto& pair : endPoints) {
    glm::vec3 start = pair.first;
    glm::vec3 end = pair.second;

    // Add endpoints to the maps
    if (m_NodeToPoint.find(start) == m_NodeToPoint.end()) {
      m_nextVertex++;
      m_NodeToPoint[start] = m_nextVertex;
      m_PointToNode[m_nextVertex] = start;
      totalNodes++;
      m_waypoints.push_back(m_nextVertex);
    }
    if (m_NodeToPoint.find(end) == m_NodeToPoint.end()) {
      m_nextVertex++;
      m_NodeToPoint[end] = m_nextVertex;
      m_PointToNode[m_nextVertex] = end;
      totalNodes++;
      m_waypoints.push_back(m_nextVertex);
    }

    // Add endpoint pair to endPoints list
    m_endPoints.push_back(std::pair<int, int>(m_NodeToPoint[start], m_NodeToPoint[end]));

    m_graph.addVertices(2);

    // Create an edge between the two endpoints if they are visible
    if (Obstacle::isVisibleExternal(start, end, obstacles)) {
      m_graph.addEdge(m_NodeToPoint[start], m_NodeToPoint[end], glm::distance(start, end));
      edges.push_back(
          std::pair<glm::vec2, glm::vec2>(glm::vec2(start.x, start.z), glm::vec2(end.x, end.z)));
    }

    // For each obstacle, check if the edge between the start and end point to
    // each obstacle point is visible, if so create an edge
    for (auto& obstacle : obstacles) {
      // TODO: Optimize
      for (auto& edge : obstacle.getBounds()) {
        if (obstacle.isVisible(start, edge.point1, obstacles)) {
          m_graph.addEdge(m_NodeToPoint[start], m_NodeToPoint[edge.point1],
                          glm::distance(start, edge.point1));
          edges.push_back(std::pair<glm::vec2, glm::vec2>(glm::vec2(start.x, start.z),
                                                          glm::vec2(edge.point1.x, edge.point1.z)));
        }
        if (obstacle.isVisible(end, edge.point1, obstacles)) {
          m_graph.addEdge(m_NodeToPoint[end], m_NodeToPoint[edge.point1],
                          glm::distance(end, edge.point1));
          edges.push_back(std::pair<glm::vec2, glm::vec2>(glm::vec2(end.x, end.z),
                                                          glm::vec2(edge.point1.x, edge.point1.z)));
        }
      }
    }
  }
}

void VisibilityGraph::clearEndPoints()
{
  for (int waypoint : m_waypoints) {
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [this, waypoint](auto& e) {
                                 return e.first == glm::vec2(m_PointToNode[waypoint].x,
                                                             m_PointToNode[waypoint].z) ||
                                        e.second == glm::vec2(m_PointToNode[waypoint].x,
                                                              m_PointToNode[waypoint].z);
                               }),
                edges.end());
    m_NodeToPoint.erase(m_PointToNode[waypoint]);
    m_PointToNode.erase(waypoint);
  }
  m_graph.removeVertices(m_waypoints);
  m_nextVertex -= m_waypoints.size();
  m_waypoints.clear();
  m_endPoints.clear();
}

int VisibilityGraph::minDistance(int dist[], bool sptSet[], int V)
{

  // Initialize min value
  int min = INT_MAX, min_index;

  for (int v = 0; v < V; v++)
    if (sptSet[v] == false && dist[v] <= min)
      min = dist[v], min_index = v;

  return min_index;
}

void VisibilityGraph::getDisjkstraPaths(std::vector<std::vector<glm::vec3>>& paths)
{
  for (auto& pathPoint : m_endPoints) {
    std::vector<glm::vec3> path;
    int src = pathPoint.first;
    int dst = pathPoint.second;

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
            shortestDistances[curr] + m_graph.getEdge(curr, j) < shortestDistances[j]) {
          // Update the shortest distance
          shortestDistances[j] = shortestDistances[curr] + m_graph.getEdge(curr, j);
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

    for (auto& i : parents) {
      // std::cout << i << ", " << std::endl;
    }

#if 0
    for (auto& i : path) {
      std::cout << i.x << ", " << i.y << ", " << i.z << std::endl;
    }
#endif
    paths.push_back(path);
  }
}
