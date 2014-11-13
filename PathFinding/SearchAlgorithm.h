#pragma once
#include <functional>
#include <vector>
#include "Vertex.h"

class SearchAlgorithm {
  public:
    typedef std::function<float(const Vertex&)> SearchHeuristic;
    virtual std::vector<const Vertex*> FindPath(const Vertex *start, SearchHeuristic h) = 0;
    virtual ~SearchAlgorithm() { }
};