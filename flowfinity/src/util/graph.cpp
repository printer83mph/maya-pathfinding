#include "flowfinity/util/graph.h"

#include <algorithm>

Graph::Graph(int vertices) : vertices(vertices), adjMatrix()
{
  for (int i = 0; i < vertices; i++) {
    adjMatrix.push_back(std::vector<float>());
    for (int j = 0; j < vertices; j++) {
      adjMatrix.back().push_back(0);
    }
  }
}

void Graph::addEdge(int src, int dest, float weight)
{
  adjMatrix.at(src).at(dest) = weight;
  adjMatrix.at(dest).at(src) = weight;
}

void Graph::removeVertices(std::vector<int> verticesToDel)
{
  // sort the array by integer value
  std::sort(verticesToDel.begin(), verticesToDel.end());
  // remove the vertices in reverse order
  for (int i = verticesToDel.size() - 1; i >= 0; i--) {
    for (auto& row : adjMatrix) {
      row.erase(row.begin() + verticesToDel[i]);
    }
    adjMatrix.erase(adjMatrix.begin() + verticesToDel[i]);
    vertices--;
  }
}

void Graph::addVertices(int verticesInput)
{
  // With the total unique endpoints, add them to the map
  vertices += verticesInput;
  // Add the new rows
  for (int i = 0; i < verticesInput; i++) {
    adjMatrix.push_back(std::vector<float>());
    adjMatrix.back().resize(vertices);
  }
  // Update the old rows
  for (int i = 0; i < vertices - verticesInput; i++) {
    adjMatrix[i].resize(vertices);
  }
}

void Graph::removeEdge(int src, int dest) { addEdge(src, dest, 0); }

float Graph::getEdge(int src, int dest) const { return adjMatrix[src][dest]; }

const int Graph::getVertices() const { return vertices; }
