#include "stdafx.h"
#include "Line.h"
#include "Util.h"

Line::Line(const Vector2f &point, const Vector2f &direction) :
    point(point), direction(direction) {
  Util::Normalize(this->direction);
}

float Line::Distance(const Vector2f &point) const {
  Vector2f vec = point - this->point;
  float lengthSquared = Util::Length2(vec);
  float projectedLength = Util::Dot(vec, direction);
  return std::sqrtf(lengthSquared - projectedLength*projectedLength);
}

Vector2f Line::Perpendicular() const {
  return Util::Perpendicular(direction);
}

float Line::NearestPointParam(const Vector2f &point) const {
  return Util::Dot(point - this->point, direction);
}

Vector2f Line::Intersection(const Line &line) const {
  assert(!IsParallel(line));
  float n, d;
  if (std::abs(line.direction.x) < 1e-6) {
    n = line.point.x - point.x + (point.y - line.point.y) * line.direction.x / line.direction.y;
    d = direction.x - line.direction.x * direction.y / line.direction.y;
  } else {
    n = line.point.y - point.y + (point.x - line.point.x) * line.direction.y / line.direction.x;
    d = direction.y - line.direction.y * direction.x / line.direction.x;
  }
  float t = n / d;
  assert(t == t);
  return point + t * direction;
}

bool Line::IsParallel(const Line &line) const {
  return direction == line.direction || direction == -line.direction;
}