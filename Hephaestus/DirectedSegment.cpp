#include "stdafx.h"
#include "DirectedSegment.h"
#include "Util.h"
#include "Interval.h"

DirectedSegment::DirectedSegment(const Vector2f &start, const Vector2f &end) {
  this->start = start;
  direction = end - start;
  length = Util::Length(direction);
  Util::Normalize(direction);
}

float DirectedSegment::CollisionDistance(const Vector2f &p,
                                         float radius) const {
  float result = FLT_MAX;
  Vector2f v(p - start);
  float dist = Util::Length(v);
  float projectedDist = Util::Dot(v, direction);
  if (projectedDist > 0) {
    float radiusSquared = radius*radius;
    float distFromLineSquared = dist*dist - projectedDist*projectedDist;
    if (distFromLineSquared <= radiusSquared) {
      float distToCollision =
          projectedDist - std::sqrt(radiusSquared - distFromLineSquared);

      if (distToCollision < 0.f) distToCollision = 0.f;
      result = distToCollision;
    }
  }
  return result;
}


Vector2f *DirectedSegment::CollisionPoint(const Vector2f &p,
                                          float radius) const {
  Vector2f *result = NULL;

  float distToCollision = CollisionDistance(p, radius);
  if (distToCollision < length) {
    result = new Vector2f(start + direction*distToCollision);
  }

  return result;
}

float DirectedSegment::CollisionDistance(std::vector<Vector2f> points,
                                         float radius) const {
  float result = FLT_MAX;
  std::vector<Vector2f>::const_iterator it = points.begin();
  while (it != points.end()) {
    float distance = CollisionDistance(*it++, radius);
    if (distance < result && distance >= 0) result = distance;
  }
  return result;
}

float DirectedSegment::CollisionDistance(const LineSegment &segment,
                                         float radius) const {
  float result = FLT_MAX;
  Line line = segment.GetLine();
  float perpendicularDist =
      Util::Dot(start - segment.p1, line.Perpendicular());
  float perpendicularSpeed =
      Util::Dot(direction, -line.Perpendicular());
  float collisionDistance = perpendicularDist/perpendicularSpeed
      - std::abs(radius/perpendicularSpeed);
  Vector2f intersection(start + collisionDistance*direction);
  Vector2f segmentVec = segment.p2 - segment.p1;
  float segmentLength = Util::Length(segmentVec);
  Util::Normalize(segmentVec);
  float projectedIntersection = Util::Dot(intersection - segment.p1, segmentVec);
  bool intersectsSegment = Interval(0, segmentLength).Contains(projectedIntersection);

  if (intersectsSegment && collisionDistance >= 0) result = collisionDistance;
  
  float endpointDistance = std::min(CollisionDistance(segment.p1, radius),
      CollisionDistance(segment.p2, radius));

  result = std::min(result, endpointDistance);
  return result;
}

Vector2f *DirectedSegment::CollisionPoint(const Rect &rectangle,
                                          float radius) const {
  Vector2f *result = NULL;

  std::vector<float> distances;
  std::vector<Vector2f> corners = rectangle.Corners();
  
  std::vector<Vector2f>::const_iterator it1 = corners.begin();
  std::vector<Vector2f>::const_iterator it2 = corners.begin() + 1;
  while (it2 != corners.end()) {
    distances.push_back(CollisionDistance(LineSegment(*it1++, *it2++), radius));
  }
  distances.push_back(CollisionDistance(LineSegment(corners.front(),
      corners.back()), radius));


  float distToCollision = *std::min_element(distances.begin(), distances.end());
  if (distToCollision < length) {
    result = new Vector2f(start + distToCollision*direction);
  }

  return result;
}