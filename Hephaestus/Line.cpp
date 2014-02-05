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