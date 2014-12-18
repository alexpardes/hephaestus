#include <Hephaestus/LineSegment.h>
#include <Hephaestus/Poly.h>
#include <Hephaestus/SectorMap.h>
#include "AStarSearch.h"
#include "PathFinder.h"
#include "SpatialVertex.h"

namespace PathFinder {

const std::vector<const Edge> PointVertex(
    const Vertex &v,
    const std::vector<const Poly> &walls,
    const Vector2f &goal) {

  std::vector<const Edge> edges;
  SectorMap sectorMap;
  auto point = static_cast<const SpatialVertex&>(v).Point();
  sectorMap.Create(point, walls);
  if (sectorMap.IsVisible(goal))
    edges.push_back(Edge(new SpatialVertex(goal), Util::Distance(point, goal)));

  return edges;
}

float Distance(const Vertex &v, const Vector2f &goal) {
  return Util::Distance(static_cast<const SpatialVertex &>(v).Point(), goal);
}

/* If the goal is inside a wall polygon, returns the nearest point on that
 * polygon, otherwise returns the original goal. */
Vector2f ClosestReachablePoint(const Vector2f &point, const std::vector<const Poly> &walls) {
  for (auto poly : walls) {
    if (poly.Contains(point))
      return poly.NearestPoint(point);
  }
  return point;
}

std::deque<const Vector2f> Path(
    const SpatialVertex *start,
    const std::vector<const Poly> &walls,
    const Vector2f &goal) {

  auto reachableGoal = ClosestReachablePoint(goal, walls);
  auto heuristic = std::bind(Distance, std::placeholders::_1, reachableGoal);
  auto adjacencies = std::bind(PointVertex, std::placeholders::_1, walls, reachableGoal);
  auto path = AStarSearch().FindPath(start, heuristic, adjacencies);
  std::deque<const Vector2f> result;
  for (auto vertex : path) {
    result.push_back(static_cast<const SpatialVertex*>(vertex)->Point());
  }
  return result;
}

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
                                        const std::vector<const Poly> &walls,
                                        const std::vector<const LineSegment> &segments) {
  std::vector<const Edge> edges;
  auto vertexPoint = static_cast<const SpatialVertex&>(v).Point();
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

std::deque<const Vector2f> Path(
    const SpatialVertex *start,
    const std::vector<const Poly> &walls,
    const std::vector<const LineSegment> &segments) {

  auto heuristic = std::bind(MinDistance, std::placeholders::_1, segments);
  auto adjacencies = std::bind(EndVertex, std::placeholders::_1, walls, segments);
  auto path = AStarSearch().FindPath(start, heuristic, adjacencies);
  std::deque<const Vector2f> result;
  for (auto vertex : path) {
    result.push_back(static_cast<const SpatialVertex*>(vertex)->Point());
  }
  return result;
}
}