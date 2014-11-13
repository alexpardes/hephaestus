#include "GridSubgoalVertex.h"

GridSubgoalVertex::GridSubgoalVertex(const SubgoalPathFinder &graph, int index) :
  graph(graph), index(index) { }

std::vector<Edge> GridSubgoalVertex::Adjacencies() const {

}