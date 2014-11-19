#pragma once
#include <functional>
#include <vector>
#include "Vertex.h"

class SearchAlgorithm {
  public:
    typedef std::function<float(const Vertex&)> SearchHeuristic;
    typedef std::function<const std::vector<const Edge>(const Vertex&)> AdjacencyList;

    // Additional adjacencies can be given in the optional argument.
    virtual std::vector<const Vertex*> FindPath(const Vertex *start, SearchHeuristic h,
        AdjacencyList adjacencies = nullptr) = 0;

    virtual ~SearchAlgorithm() { }
};