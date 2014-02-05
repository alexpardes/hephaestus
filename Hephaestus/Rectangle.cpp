#include "stdafx.h"
#include "Rectangle.h"
#include "Util.h"

std::vector<Vector2f> Rect::Corners() const {
  std::vector<Vector2f> corners;
  corners.push_back(topLeft);
  corners.push_back(Vector2f(topLeft.x, bottomRight.y));
  corners.push_back(bottomRight);
  corners.push_back(Vector2f(bottomRight.x, topLeft.y));
  return corners;
}

std::vector<LineSegment> Rect::Sides() const {
  std::vector<LineSegment> sides;
  std::vector<Vector2f> corners = Corners();
  for (size_t i = 0; i < corners.size() - 1; ++i) {
    sides.push_back(LineSegment(corners[i], corners[i + 1]));
  }
  sides.push_back(LineSegment(corners.front(), corners.back()));
  return sides;
}

Vector2f Rect::Center() const {
  return (topLeft + bottomRight) / 2.f;
}

Vector2f Rect::NearestPoint(const Vector2f &point) const {
  Vector2f result;
  float minimumDistance = FLT_MAX;

  std::vector<LineSegment> sides = Sides();
  for (LineSegment side : sides) {
    Vector2f closestPoint = side.NearestPoint(point);
    float distance = Util::Distance(point, closestPoint);
    if (distance < minimumDistance) {
      minimumDistance = distance;
      result = closestPoint;
    }
  }

  return result;
}