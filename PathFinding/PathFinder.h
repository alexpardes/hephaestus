#pragma once
#include <deque>
#include <vector>
#include <Hephaestus/Vector2.h>

class Poly;
class LineSegment;
class SpatialVertex;
class Waypoint;

namespace PathFinder {
  std::deque<const Vector2f> Path(
    const SpatialVertex *start,
    const std::vector<const Poly> &walls,
    const Vector2f &goal);

  std::deque<const Vector2f> Path(
    const SpatialVertex *start,
    const std::vector<const Poly> &walls,
    const std::vector<const LineSegment> &segments);
};