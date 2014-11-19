#include <Hephaestus/SectorMap.h>
#include "SpatialGraph.h"
#include "SpatialVertex.h"

SpatialGraph::SpatialGraph(const std::vector<Vector2f> &points,
                           const std::vector<const std::vector<int>> &adjacencyLists) {

  for (auto point : points) {
    vertices.push_back(std::unique_ptr<SpatialVertex>(new SpatialVertex(point)));
  }

  for (size_t i = 0; i < adjacencyLists.size(); ++i) {
    for (auto index : adjacencyLists[i]) {
      vertices[i]->AddAdjacency(vertices[index].get());
    }
  }
}

std::unique_ptr<const SpatialVertex> SpatialGraph::MakeVertex(
  const Vector2f &point,
  const SectorMap &sectorMap) const {

  auto resultVertex = new SpatialVertex(point);
  for (auto vertex = vertices.begin(); vertex != vertices.end(); ++vertex) {
    if (sectorMap.IsVisible((*vertex)->Point()))
      resultVertex->AddAdjacency(vertex->get());
  }
  return std::unique_ptr<const SpatialVertex>(resultVertex);
}