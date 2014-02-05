#include "stdafx.h"
#include "LineSegment.h"
#include "Util.h"

Vector2f LineSegment::NearestPoint(const Vector2f &point) const {
  float t = GetLine().NearestPointParam(point);
  if (t < 0) return p1;
  if (t > Length()) return p2;
  return GetLine().Evaluate(t);
}

float LineSegment::Length() const {
  return Util::Length(p1 - p2);
}