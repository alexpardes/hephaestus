#include <Hephaestus/LineSegment.h>
#include <Hephaestus/SectorMap.h>
#include <Hephaestus/Waypoint.h>
#include "AStarSearch.h"
#include "PolygonPathFinder.h"
#include "SpatialVertex.h"

float MinDistance(const Vertex &v, const std::vector<const LineSegment> &segments) {
  auto point = static_cast<const SpatialVertex&>(v).Point();
  float minDistance = FLT_MAX;
  for (auto segment: segments) {
    float distance = segment.Distance(point);
    minDistance = std::min(distance, minDistance);
  }
  return minDistance;
}

const std::vector<const Edge> EndVertex(const Vertex &v,
                                        const std::vector<const LineSegment> &walls,
                                        const std::vector<const LineSegment> &segments) {
  std::vector<const Edge> edges;
  auto  vertexPoint = static_cast<const SpatialVertex&>(v).Point();
  SectorMap sectorMap;
  sectorMap.Create(vertexPoint, walls);
  auto subsegments = sectorMap.VisibleSubsegments(segments);
  if (subsegments.empty())
    return edges;

  float minDist = FLT_MAX;
  Vector2f closestPoint;
  for (auto segment : subsegments) {
    auto point = segment.NearestPoint(vertexPoint);
    auto distance = Util::Distance(point, vertexPoint);
    if (distance < minDist) {
      minDist = distance;
      closestPoint = point;
    }
  }
  // TODO: Fix memory leak.
  edges.push_back(Edge(new SpatialVertex(closestPoint), minDist));
  return edges;
}



std::deque<const Waypoint*> PolygonPathFinder::Path(
  const SpatialVertex *start,
  const std::vector<const LineSegment> &walls,
  const std::vector<const LineSegment> &segments) {

  auto path = AStarSearch().FindPath(start, std::bind(MinDistance, std::placeholders::_1, segments), std::bind(EndVertex, std::placeholders::_1, walls, segments));
  std::deque<const Waypoint*> result;
  for (auto vertex : path) {
    result.push_back(new Waypoint(static_cast<const SpatialVertex*>(vertex)->Point()));
  }
  return result;
}