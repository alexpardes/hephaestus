#include "PathFinder.h"
#include <cmath>

float PathingGrid::OctileDistance(const Vector2i &p1,
                                        const Vector2i &p2) {
  int dx = std::abs(p1.x - p2.x);
  int dy = std::abs(p1.y - p2.y);
  return std::max(dx, dy) + (std::sqrtf(2) - 1) * std::min(dx, dy);
}