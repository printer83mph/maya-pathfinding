#pragma once

#include <vector>

// Graph represented with adjacency matrix
class Graph {
public:
  Graph(int vertices);
  void addEdge(int src, int dest, float weight);
  void removeEdge(int src, int dest);
  float getEdge(int src, int dest) const;
  const int getVertices() const;

  friend class FlowFinity;

private:
  int vertices;
  std::vector<std::vector<float>> adjMatrix;
};