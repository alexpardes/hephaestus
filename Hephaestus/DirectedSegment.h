#pragma once
#include <vector>
#include "Vector2.h"
#include "Rectangle.h"
#include "LineSegment.h"

// Represents a line segment with a direction.
class DirectedSegment {
  public:
    DirectedSegment(const Vector2f &start, const Vector2f &end);

    // Returns the first point on the segment which is within radius of p.
    Vector2f *CollisionPoint(const Vector2f &p, float radius) const;
    float CollisionDistance (std::vector<Vector2f> points, float radius) const;
    Vector2f *CollisionPoint(const Rect &rectangle, float radius) const;
    float CollisionDistance(const Vector2f &p, float radius) const;
    float CollisionDistance(const LineSegment &segment, float radius) const;
    Vector2f Start() const { return start; }
    Vector2f End() const { return start + length*direction; }

  private:
    Vector2f start, direction;
    float length;
};