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
          --j;
        }
      }
    }
  }

  // Return value would be a list of points if doing more accurate tessellation.
  Vector2f Dilate(const Poly::Vertex &vertex, float radius) {
    float distance = radius / std::sinf(vertex.InteriorAngle() / 2);
    return vertex.Point() + distance * vertex.Normal();
  }

  Poly Dilate(const Poly &polygon, float radius) {
    Poly dilatedPolygon;
    for (auto vertex : polygon) {
      dilatedPolygon.Add(Dilate(vertex, radius));
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