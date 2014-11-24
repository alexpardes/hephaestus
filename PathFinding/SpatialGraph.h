#pragma once
#include <vector>

class Poly;
class SectorMap;
class SpatialVertex;

class SpatialGraph {
  public:
    SpatialGraph(const std::vector<const Poly> &polygons);
    std::unique_ptr<const SpatialVertex> MakeVertex(const Vector2f &point) const;
    const std::vector<const Poly> &Walls() const;

  private:
    std::vector<const std::unique_ptr<SpatialVertex>> vertices;
    std::vector<const Poly> polygons;
};