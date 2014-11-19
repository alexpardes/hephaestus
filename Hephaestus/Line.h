#pragma once
#include "Vector2.h"

class Line {
  public:
    Line(const Vector2f &point, const Vector2f &direction);
    float Distance(const Vector2f &point) const;
    Vector2f Direction() const { return direction; }
    Vector2f Perpendicular() const;
    Vector2f Evaluate(float t) const { return point + t*direction; }
    float NearestPointParam(const Vector2f &point) const;
    Vector2f Intersection(const Line &line) const;

  private:
    Vector2f point, direction;
};