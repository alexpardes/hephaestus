#pragma once
#include <vector>
#include "Vector2.h"

struct Rect {
    Vector2f topLeft, bottomRight;
    Rect() { }
    Rect(const Vector2f &topLeft, const Vector2f &bottomRight) :
      topLeft(topLeft), bottomRight(bottomRight) { }
    std::vector<Vector2f> Corners() const;
    Vector2f Center() const;
};