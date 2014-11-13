#pragma once
#include <vector>
#include "Vertex.h"

class SearchAlgorithm {
  public:
    typedef float (*SearchHeuristic)(const Vertex&);
    virtual std::vector<const Vertex*> FindPath(const Vertex *start, SearchHeuristic h) = 0;
    virtual ~SearchAlgorithm() { }
};