#pragma once
#include "Vector2.h"
#include "Line.h"

class LineSegment {
  public:
    LineSegment(const Vector2f &p1, const Vector2f &p2) : p1(p1), p2(p2) { }
    Line GetLine() const { return Line(p1, p2 - p1); }
    Vector2f NearestPoint(const Vector2f &point) const;
    float Length() const;
    Vector2f p1, p2;
};