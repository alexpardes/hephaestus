#include "stdafx.h"
#include "Rectangle.h"

std::vector<Vector2f> Rect::Corners() const {
  std::vector<Vector2f> corners;
  corners.push_back(topLeft);
  corners.push_back(Vector2f(topLeft.x, bottomRight.y));
  corners.push_back(bottomRight);
  corners.push_back(Vector2f(bottomRight.x, topLeft.y));
  return corners;
}

Vector2f Rect::Center() const {
  return (topLeft + bottomRight) / 2.f;
}