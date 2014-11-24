#pragma once
#include <vector>
#include "Poly.h"

namespace MinkowskiSum {
  std::vector<const Poly> Dilate(const std::vector<const Poly> &polygons, float radius);
};