#include <Hephaestus/Poly.h>
#include <Hephaestus/SectorMap.h>
#include <Hephaestus/MinkowskiSum.h>
#include "SpatialGraph.h"
#include "SpatialVertex.h"

SpatialGraph::SpatialGraph(const std::vector<const Poly> &polygons) {
  this->polygons = polygons;
  dilatedPolygons = MinkowskiSum::Dilate(polygons, 25);
  for (auto polygon : dilatedPolygons) {
    for (auto vertex : polygon) {
      if (vertex.IsConvex())
        vertices.push_back(std::unique_ptr<SpatialVertex>(new SpatialVertex(vertex.Point())));
    }
  }

  auto polygonIt = dilatedPolygons.begin();
  int vertexIndex = 0;
  for (size_t i = 0; i < vertices.size(); ++i) {
    SectorMap map;
    map.Create(vertices[i]->Point(), dilatedPolygons);
    bool isFirstVertex = vertexIndex == 0;
    auto lastVertexIndex = polygonIt->Size() - 1;
    bool isLastVertex = vertexIndex == lastVertexIndex;
    for (size_t j = i + 1; j < vertices.size(); ++j) {
      bool areAdjacent = j == i + 1 && !isLastVertex || j == i + lastVertexIndex && isFirstVertex;
      if (areAdjacent || map.IsVisible(vertices[j]->Point())) {
        vertices[i]->AddAdjacency(vertices[j].get());
        vertices[j]->AddAdjacency(vertices[i].get());
      }
    }

    ++vertexIndex;
    if (vertexIndex == polygonIt->Size()) {
      vertexIndex = 0;
      ++polygonIt;
    }
  }
}

std::unique_ptr<const SpatialVertex> SpatialGraph::MakeVertex(const Vector2f &point) const {
  SectorMap sectorMap;
  sectorMap.Create(point, dilatedPolygons);
  auto resultVertex = new SpatialVertex(point);
  for (auto vertex = vertices.begin(); vertex != vertices.end(); ++vertex) {
    if (sectorMap.IsVisible((*vertex)->Point()))
      resultVertex->AddAdjacency(vertex->get());
  }
  return std::unique_ptr<const SpatialVertex>(resultVertex);
}

const std::vector<const Poly> &SpatialGraph::Walls() const {
  return polygons;
}

const std::vector<const Poly> &SpatialGraph::DilatedWalls() const {
  return dilatedPolygons;
}