#pragma once
#include "Vector2.h"
#include <vector>
#include "Util.h"

class Circle {
  public:
    Circle(const Vector2f& center, float radius) :
        center(center), radius(radius) { }

    std::pair<Vector2f, Vector2f> WidestPoints(const Vector2f &point) const {
      std::pair<Vector2f, Vector2f> result;
      Vector2f direction = center - point;
      Vector2f perpendicular = Util::Normalized(Util::Perpendicular(direction));
      Vector2f point1 = center + radius * perpendicular;
      Vector2f point2 = center - radius * perpendicular;

      float angle1 = Util::FindAngle(point1 - point);
      float angle2 = Util::FindAngle(point2 - point);

      if (Util::AngleCCW(angle1, angle2) <= M_PI) {
        result.first = point1;
        result.second = point2;
      } else {
        result.first = point2;
        result.second = point1;
      }

      return result;
    }

  private:
    Vector2f center;
    float radius;
};