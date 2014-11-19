#include <Hephaestus/Util.h>
#include "Subgoal.h"
#include "GridRegion.h"

Subgoal::Subgoal(const Vector2i &point, const Vector2i &direction) {
  this->point = point + direction;
  this->direction = direction;
  region = new GridRegion(GridRegion::GetIndirect(Vector2i(point),
      this->point));
}

std::vector<const Edge> Subgoal::Adjacencies() const {
  std::vector<const Edge> adjacencies;
  for (auto subgoal : adjacencyList) {
    float cost = Util::Distance(Util::ToVector2f(point), Util::ToVector2f(subgoal->point));
    adjacencies.push_back(Edge(subgoal, cost));
  }
  return adjacencies;
}

bool Subgoal::IsPast(const Vector2i &point) const {
  return region->Contains(point);
}

Subgoal::~Subgoal() {
  delete region;
}