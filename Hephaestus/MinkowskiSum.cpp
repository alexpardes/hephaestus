#include "stdafx.h"
#include <unordered_set>
#include "MinkowskiSum.h"
#include "Vector2.h"

namespace MinkowskiSum {

  /* Intersection should be a point on segment. Returns true if there is an
   * intersection between segment and polygon after the input intersection.
   * If true, vertex will be set to the start vertex of the intersected segment
   * on the polygon, and intersection will be set to the intersection point. */
  bool FirstIntersection(
      const LineSegment &segment,
      const Poly &polygon,
      Poly::Vertex &vertex,
      Vector2f &intersection) {

    const float epsilon = 0.001f;

    float startParam = segment.GetLine().NearestPointParam(intersection);;
    float minParam = FLT_MAX;
    for (auto tempVertex : polygon) {
      Vector2f tempIntersection;
      float param;
      if (segment.Intersects(tempVertex.Segment(), tempIntersection, param) && param > startParam + epsilon && param < minParam) {
        minParam = param;
        intersection = tempIntersection;
        vertex = tempVertex;
      }
    }
    return minParam < FLT_MAX;
  }

  // Not sure this will handle polygons which intersect at a vertex.
  Poly CombineIntersections(const Poly &polygon1, const Poly &polygon2) {
    std::unordered_set<Poly::Vertex> visitedVertices;
    std::vector<Vector2f> points;
    bool areIntersecting = false;
    auto currentVertex = polygon1.begin();
    while (polygon2.Contains(currentVertex.Point())) {
      if (++currentVertex == polygon1.end())
        return CombineIntersections(polygon2, polygon1);
    }

    auto currentPoint = currentVertex.Point();
    auto currentPolygon = &polygon1;
    auto otherPolygon = &polygon2;
    bool atVertex = true;

    while (!atVertex || !visitedVertices.count(currentVertex)) {
      points.push_back(currentPoint);
      if (atVertex)
        visitedVertices.insert(currentVertex);

      auto segment = currentVertex.Segment();
      Poly::Vertex vertex;
      if (FirstIntersection(segment, *otherPolygon, vertex, currentPoint)) {
        areIntersecting = true;
        std::swap(currentPolygon, otherPolygon);
        currentVertex = vertex;
        atVertex = false;
      } else {
        currentVertex = currentVertex.Next();
        currentPoint = currentVertex.Point();
        atVertex = true;
      }
    }

    Poly result;
    if (!areIntersecting)
      return result;

    result.SetReversed(polygon1.IsReversed() || polygon2.IsReversed());

    auto pointIt = points.begin();
    while (pointIt != points.end() && *pointIt != currentPoint)
      ++pointIt;

    for (; pointIt != points.end(); ++pointIt) {
      result.Add(*pointIt);
    }
    return result;
  }

  void CombineIntersections(std::vector<const Poly> &polygons) {
    for (size_t i = 0; i < polygons.size(); ++i) {
      for (size_t j = i + 1; j < polygons.size(); ++j) {
        auto combined = CombineIntersections(polygons[i], polygons[j]);
        if (combined.Size()) {
          polygons[i] = combined;        
          polygons.erase(polygons.begin() + j);
          j = i; // Have to test for intersections with everything polygons[j] had not been tested against.
        }
      }
    }
  }

  std::vector<Vector2f> Dilate(const Poly::Vertex &vertex, float radius) {
    std::vector<Vector2f> result;
    auto angle = vertex.InteriorAngle() / 2;
    if (angle >= M_PI / 3) {
      // This case returns the intersection of the dilated segments containing the vertex.
      float distance = radius / std::sinf(angle);
      result.push_back(vertex.Point() + distance * vertex.Normal());
    } else {
      /* In this case a single point is a poor approximation. This returns the
       * two points on the the dilated segments such that the resulting polygon
       * fully contains the exact dilation. */
      auto phi = (M_PI - angle) / 4;
      auto length = radius / std::sinf(phi);
      auto v = length * vertex.Normal();
      auto p1 = vertex.Point() + Util::Rotated(v, -phi);
      auto p2 = vertex.Point() + Util::Rotated(v, phi);
      result.push_back(p1);
      result.push_back(p2);
    }
    return result;
  }

  Poly Dilate(const Poly &polygon, float radius) {
    Poly dilatedPolygon;
    dilatedPolygon.SetReversed(polygon.IsReversed());
    for (auto vertex : polygon) {
      for (auto dilatedVertex : Dilate(vertex, radius)) {
        dilatedPolygon.Add(dilatedVertex);
      }
    }
    return dilatedPolygon;
  }

  std::vector<const Poly> Dilate(const std::vector<const Poly> &polygons, float radius) {
    std::vector<const Poly> dilatedPolygons;
    for (auto polygon : polygons) {
      dilatedPolygons.push_back(Dilate(polygon, radius));
    }
    CombineIntersections(dilatedPolygons);
    return dilatedPolygons;
  }
}