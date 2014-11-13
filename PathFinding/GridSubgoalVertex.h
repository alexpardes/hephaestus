#pragma once
#include "Vertex.h"

class SubgoalPathFinder;

class GridSubgoalVertex : public Vertex {
  public:
    GridSubgoalVertex(const SubgoalPathFinder &graph, int index);
    virtual std::vector<Edge> Adjacencies() const;

  private:
    int index;
    const SubgoalPathFinder &graph;
};