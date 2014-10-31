#include "stdafx.h"
#include "Rectangle.h"
#include "Util.h"

boost::array<Vector2f, 4> Rect::Corners() const {
  boost::array<Vector2f, 4> corners;
  corners[0] = topLeft;
  corners[1] = Vector2f(topLeft.x, bottomRight.y);
  corners[2] = bottomRight;
  corners[3] = Vector2f(bottomRight.x, topLeft.y);
  return corners;
}

std::vector<LineSegment> Rect::Sides() const {
  std::vector<LineSegment> sides;
  auto corners = Corners();
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

  auto sides = Sides();
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

std::pair<Vector2f, Vector2f> Rect::WidestPoints(const Vector2f& point) const {
  auto corners = Corners();
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

  return std::make_pair(rightmost, leftmost);
}