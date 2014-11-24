#include "stdafx.h"
#include "LineSegment.h"
#include "Util.h"

Vector2f LineSegment::NearestPoint(const Vector2f &point) const {
  float t = GetLine().NearestPointParam(point);
  if (t < 0) return p1;
  if (t > Length()) return p2;
  return GetLine().Evaluate(t);
}

float LineSegment::Distance(const Vector2f &point) const {
  return Util::Distance(point, NearestPoint(point));
}

float LineSegment::Length() const {
  return Util::Length(p1 - p2);
}

bool LineSegment::Intersects(const LineSegment &segment) const {
  Vector2f intersection;
  float param;
  return Intersects(segment, intersection, param);
}

bool LineSegment::Intersects(const LineSegment &segment, Vector2f &intersection, float &param) const {
  auto line1 = GetLine();
  auto line2 = segment.GetLine();
  if (line1.IsParallel(line2))
    return false;

  intersection = line1.Intersection(line2);
  param = line1.NearestPointParam(intersection);
  auto t = line2.NearestPointParam(intersection);
  bool isOnSegment1 = param >= 0 && param <= Length();
  bool isOnSegment2 = t >= 0 && t <= segment.Length();
  return isOnSegment1 && isOnSegment2;
}