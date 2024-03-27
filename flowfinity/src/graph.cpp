#include <graph.h>

Graph::Graph(int vertices) : vertices(vertices), adjMatrix() {
  for (int i = 0; i < vertices; i++) {
    adjMatrix.push_back(std::vector<float>());
    for (int j = 0; j < vertices; j++)
      adjMatrix.back().push_back(0);
  }
}

void Graph::addEdge(int src, int dest, float weight) {
  adjMatrix.at(src).at(dest) = weight;
  adjMatrix.at(dest).at(src) = weight;
}

void Graph::removeEdge(int src, int dest) { addEdge(src, dest, 0); }

float Graph::getEdge(int src, int dest) const { return adjMatrix[src][dest]; }