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

void Rect::Grow(float distance) {
  Vector2f movement(distance, distance);
  topLeft -= movement;
  bottomRight += movement;
}

std::vector<Vector2f> Rect::WidestPoints(const Vector2f& point) const {
  std::vector<Vector2f> corners = Corners();
  Vector2f leftmost = corners[0];
  Vector2f rightmost = corners[0];
  float minAngle = Util::FindAngle(corners[0] - point);
  float maxAngle = minAngle;

  for (int i = 1; i < corners.size(); ++i) {
    float angle = Util::FindAngle(corners[i] - point);
    if (Util::AngleCCW(maxAngle, angle) <= M_PI) {
      maxAngle = angle;
      leftmost = corners[i];
    } else if (Util::AngleCW(minAngle, angle) <= M_PI) {
      minAngle = angle;
      rightmost = corners[i];
    }
  }

  std::vector<Vector2f> result;
  result.push_back(rightmost);
  result.push_back(leftmost);
  return result;
}