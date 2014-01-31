#pragma once
#include <vector>
#include <Hephaestus/Vector2.h>

class GridRegion;

class Subgoal {
public:
  Subgoal(const Vector2i &point, const Vector2i &direction);
  void AddAdjacency(Subgoal *subgoal) {
    adjacencyList.push_back(subgoal);
  }

  ~Subgoal();

  const std::vector<Subgoal*> &AdjacencyList() const { return adjacencyList; }
  const Vector2i &Point() const { return point; }
  bool IsPast(const Vector2i &point) const ;

  Vector2i Direction() const { return direction; }

private:
  Vector2i point;
  Vector2i direction;
  GridRegion *region;
  std::vector<Subgoal*> adjacencyList;
};