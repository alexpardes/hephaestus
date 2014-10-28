#pragma once
#include <vector>
#include "Util.h"

class VisibilityFan {
  public:
    VisibilityFan() { }
    VisibilityFan(const Vector2f &origin, const std::vector<Vector2f> &visiblePoints) :
        origin(origin), visiblePoints(visiblePoints) { }
    const Vector2f &Origin() const { return origin; }
    const std::vector<Vector2f> &VisiblePoints() const { return visiblePoints; }

  private:
    Vector2f origin;
    std::vector<Vector2f> visiblePoints;
};