#include "AStarSearch.h"
#include <assert.h>
#include "Vertex.h"

struct SearchNode {
  SearchNode(const Vertex *vertex, const SearchNode *parent, float g, float h) :
    vertex(vertex), parent(parent), g(g), h(h) { }

  SearchNode() { }

  const Vertex *vertex;
  const SearchNode *parent;
  float g, h;
};

bool SearchNodeGreater::operator()(const SearchNode *a, const SearchNode *b) {
  return a->g + a->h > b->g + b->h;
}

void AStarSearch::Reset() {
  searchNodes.clear();
  visitedList.clear();
  while (!openList.empty())
    openList.pop();
}

std::vector<const Vertex*> AStarSearch::FindPath(const Vertex *start,
                                                 SearchHeuristic h,
                                                 AdjacencyList adjacencies) {
  Reset();
  this->heuristic = h;  
  openList.push(AddStartNode(start));
  while(!openList.empty()) {
    const SearchNode *node = openList.top();
    openList.pop();

    if (Visit(node, adjacencies)) {
      return Path(node);
    }
  }
  return std::vector<const Vertex*>();
}

const float kHeuristicEpsilon = 0.01f;

bool AStarSearch::Visit(const SearchNode *node, AdjacencyList adjacencies) {
  if (visitedList.count(node->vertex))
    return false;

  if (node->h <= kHeuristicEpsilon) {
    return true;
  }

  visitedList.insert(node->vertex);
  for (auto edge : node->vertex->Adjacencies()) {
    if (!visitedList.count(edge.vertex))
      openList.push(AddNode(edge, node));
  }

  if (adjacencies) {
    for (auto edge : adjacencies(*node->vertex)) {
      if (!visitedList.count(edge.vertex))
        openList.push(AddNode(edge, node));
    }
  }
  return false;
}

std::vector<const Vertex*> AStarSearch::Path(const SearchNode *node) const {
  const SearchNode *currentNode = node;
  std::vector<const Vertex*> path;
  while (currentNode->parent != nullptr) {
    path.push_back(currentNode->vertex);
    currentNode = currentNode->parent;
  }
  std::reverse(path.begin(), path.end());
  return path;
}

const SearchNode *AStarSearch::AddNode(Edge edge, const SearchNode *parent) {
  float g = parent->g + edge.cost;
  float h = this->heuristic(*edge.vertex);
  auto node = std::make_shared<SearchNode>(edge.vertex, parent, g, h);
  searchNodes.push_back(node);
  return node.get();
}

const SearchNode *AStarSearch::AddStartNode(const Vertex *vertex) {
  assert(searchNodes.size() == 0);
  auto node = std::make_shared<SearchNode>(vertex, nullptr, 0.f, heuristic(*vertex));
  searchNodes.push_back(node);
  return node.get();
}