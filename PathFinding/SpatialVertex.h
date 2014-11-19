#pragma once
#include <Hephaestus/Vector2.h>
#include "Vertex.h"

class PolygonPathFinder;

class SpatialVertex : public Vertex {
public:
  SpatialVertex(const Vector2f &point);
  Vector2f Point() const;
  void AddAdjacency(const SpatialVertex *vertex);
  virtual std::vector<const Edge> Adjacencies() const;

private:
  Vector2f point;
  std::vector<const SpatialVertex*> adjacencies;
};
