#pragma once
#include <vector>
#include "Vector2.h"
#include "LineSegment.h"

struct Rect {
    Vector2f topLeft, bottomRight;
    Rect() { }
    Rect(const Vector2f &topLeft, const Vector2f &bottomRight) :
      topLeft(topLeft), bottomRight(bottomRight) { }
    std::vector<Vector2f> Corners() const;
    std::vector<LineSegment> Sides() const;
    Vector2f Center() const;
    Vector2f NearestPoint(const Vector2f &point) const;
};