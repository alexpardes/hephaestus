#pragma once
#include <vector>

struct Edge;

class Vertex {
  public:
    virtual std::vector<Edge> Adjacencies() const = 0;
    virtual ~Vertex() { }
};

struct Edge {
  Edge(const Vertex *vertex, float cost) : vertex(vertex), cost(cost) { }
  const Vertex *vertex;
  float cost;
};