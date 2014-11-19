#pragma once
#include <vector>

class SectorMap;
class SpatialVertex;

class SpatialGraph {
  public:
    SpatialGraph(const std::vector<Vector2f> &points,
        const std::vector<const std::vector<int>> &adjacencyLists);

    std::unique_ptr<const SpatialVertex> MakeVertex(const Vector2f &point,
        const SectorMap &sectorMap) const;

  private:
    std::vector<const std::unique_ptr<SpatialVertex>> vertices;
};