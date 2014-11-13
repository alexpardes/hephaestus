#pragma once
#include <memory>
#include <queue>
#include <unordered_set>
#include "SearchAlgorithm.h"

struct SearchNode;
struct SearchNodeGreater {
  bool operator()(const SearchNode *a, const SearchNode *b);
};

class AStarSearch : public SearchAlgorithm {
  public:
    virtual std::vector<const Vertex*> FindPath(const Vertex *start, SearchHeuristic h);    

  private:
    std::vector<std::shared_ptr<SearchNode>> searchNodes;
    std::unordered_set<const Vertex*> visitedList;
    std::priority_queue<const SearchNode*, std::vector<const SearchNode*>, SearchNodeGreater> openList;
    SearchHeuristic heuristic;

    // Returns true iff the node's vertex is a destination (h(v) == 0).
    bool Visit(const SearchNode *node);
    std::vector<const Vertex*> Path(const SearchNode *node) const;
    void Reset();
    const SearchNode *AddNode(Edge edge, const SearchNode *parent);
    const SearchNode *AddStartNode(const Vertex *vertex);
};