#pragma once
#include <deque>
#include <vector>
#include <Hephaestus/Vector2.h>

class LineSegment;
class SpatialVertex;
class Waypoint;

class PolygonPathFinder {
  public:
    static std::deque<const Waypoint*> Path(const SpatialVertex *start,
        const std::vector<const LineSegment> &walls,
        const std::vector<const LineSegment> &segments);

  private:
    PolygonPathFinder();
};