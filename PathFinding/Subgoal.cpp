#include "Subgoal.h"
#include "GridRegion.h"

Subgoal::Subgoal(const Vector2i &point, const Vector2i &direction) {
  this->point = point + direction;
  this->direction = direction;
  region = new GridRegion(GridRegion::GetIndirect(Vector2i(point),
      this->point));
}

bool Subgoal::IsPast(const Vector2i &point) const {
  return region->Contains(point);
}

Subgoal::~Subgoal() {
  delete region;
}