#pragma once
#include <vector>
#include <boost/array.hpp>
#include "Vector2.h"
#include "LineSegment.h"

struct Rect {
    Vector2f topLeft, bottomRight;
    Rect() { }
    Rect(const Vector2f &topLeft, const Vector2f &bottomRight) :
      topLeft(topLeft), bottomRight(bottomRight) { }

    boost::array<Vector2f, 4> Corners() const;
    std::vector<LineSegment> Sides() const;
    Vector2f Center() const;
    Vector2f NearestPoint(const Vector2f &point) const;

    // Returns the rightmost and leftmost points (in that order) relative to
    // the given point.
    std::pair<Vector2f, Vector2f> WidestPoints(const Vector2f& point) const;

    // Moves each side of the rectangle outward by the given distance.
    void Grow(float distance);
};