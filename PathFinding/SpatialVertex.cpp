#include <Hephaestus/Util.h>
#include "PolygonPathFinder.h"
#include "SpatialVertex.h"

SpatialVertex::SpatialVertex(const Vector2f &point) {
  this->point = point;
}

Vector2f SpatialVertex::Point() const {
  return point;
}

std::vector<const Edge> SpatialVertex::Adjacencies() const {
  std::vector<const Edge> edges;
  for (auto vertex : adjacencies) {
    edges.push_back(Edge(vertex, Util::Distance(Point(), vertex->Point())));
  }
  return edges;
}

void SpatialVertex::AddAdjacency(const SpatialVertex *vertex) {
  adjacencies.push_back(vertex);
}