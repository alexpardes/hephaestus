#include "SubgoalPathFinder.h"
#include <queue>
#include <unordered_set>
#include <Hephaestus/Vector2.h>
#include <Hephaestus/Util.h>
#include <Hephaestus/Waypoint.h>
#include "AStarSearch.h"
#include "GridRegion.h"
#include "Subgoal.h"

SubgoalPathFinder::SubgoalPathFinder(
    const PathingGrid *grid,
    const Vector2f &tileSize,
    const std::vector<Subgoal*> &subgoals,
    const std::vector<const std::vector<int>> &adjacencyLists) : grid(grid) {

  this->tileSize = tileSize;
  this->subgoals = subgoals;

  for (int i = 0; i < subgoals.size(); ++i) {
    const std::vector<int> &adjacencyList = adjacencyLists.at(i);
    for (int j = 0; j < adjacencyList.size(); ++j) {
      Subgoal *adjacent = subgoals.at(adjacencyList.at(j));
      subgoals.at(i)->AddAdjacency(adjacent);
    }
  }
}

bool SubgoalPathFinder::CanMoveInDirection(const Vector2i &p,
                                           const Vector2i &direction) {
  return grid->IsUnblocked(p.x, p.y + direction.y)
      && grid->IsUnblocked(p.x + direction.x, p.y)
      && grid->IsUnblocked(p.x + direction.x, p.y + direction.y);
}

bool SubgoalPathFinder::AreDirectlyConnected(const Vector2i &p1,
                                             const Vector2i &p2) {
  if (p1 == p2) return true;
  Vector2i direction = GetDirection(p1, p2);
  if (!CanMoveInDirection(p1, direction)) return false;
  Vector2i nextTile = p1 + direction;
  return AreDirectlyConnected(nextTile, p2);
}

Vector2i SubgoalPathFinder::GetDirection(const Vector2i &p1,
                                         const Vector2i &p2) {
  int x, y;
  int dx = p2.x - p1.x;
  if (dx == 0) {
    x = 0;
  } else {
    x = dx > 0 ? 1 : -1;
  }

  int dy = p2.y - p1.y;
  if (dy == 0) {
    y = 0;
  } else {
    y = dy > 0 ? 1 : -1;
  }

  return Vector2i(x, y);
}

SubgoalPathFinder::~SubgoalPathFinder() {
  for (int i = 0; i < subgoals.size(); ++i) {
    delete subgoals.at(i);
  }
}

float Distance(const Vector2i &a, const Vector2i &b) {
  Vector2f af = Util::ToVector2f(a);
  Vector2f bf = Util::ToVector2f(b);
  return Util::Distance(af, bf);
}

bool SubgoalPathFinder::SubgoalComparer::operator()(Subgoal *a, Subgoal *b) {
  float distance1 = PathingGrid::OctileDistance(a->Point(), point);
  float distance2 = PathingGrid::OctileDistance(b->Point(), point);
  return distance1 < distance2;
}

std::vector<Subgoal*> SubgoalPathFinder::GetDirectSubgoals(const Vector2i &p) {

  // List of regions which are h-reachable from p through a subgoal.
  std::vector<Subgoal*> result;
  std::vector<GridRegion> indirectRegions;
  std::sort(subgoals.begin(), subgoals.end(), SubgoalComparer(p));
  for (int i = 0; i < subgoals.size(); ++i) {
    Subgoal *subgoal = subgoals.at(i);
    bool isDirect = true;
    for (int j = 0; j < indirectRegions.size(); ++j) {
      if (indirectRegions.at(j).Contains(subgoal->Point())) {
        isDirect = false;
        break;
      }
    }    
    if (isDirect && AreDirectlyConnected(p, subgoal->Point())) {
      result.push_back(subgoal);
      indirectRegions.push_back(GridRegion::GetIndirect(p, subgoal->Point()));
    }
  }
  return result;
}

Vector2i SubgoalPathFinder::GetTile(const Vector2f &position) {
  Vector2i result;
  result.x = int(position.x / tileSize.x);
  result.y = int(position.y / tileSize.y);
  return result;
}

std::vector<Waypoint*> SubgoalPathFinder::GetPath(const Vector2f &startpoint,
                                                 const Vector2f &endpoint) {

  Vector2i start = GetTile(startpoint);
  Vector2i end = GetTile(endpoint);

  std::vector<Subgoal*> startSubgoals = GetDirectSubgoals(start);
  Subgoal *startGoal = new Subgoal(start, Vector2i(0, 0));
  for (auto subgoal : startSubgoals) {
    startGoal->AddAdjacency(subgoal);
  }

  auto endGoal = new Subgoal(end, Vector2i(0, 0));
  std::vector<Subgoal*> endSubgoals = GetDirectSubgoals(end);
  for (auto subgoal : endSubgoals) {
    subgoal->AddAdjacency(endGoal);
  }
  if (AreDirectlyConnected(start, end))
    startGoal->AddAdjacency(endGoal);

  auto aStarSearch = std::unique_ptr<AStarSearch>(new AStarSearch());
  auto vertexPath = aStarSearch->FindPath(startGoal, [end](const Vertex& v) {
      return Distance(static_cast<const Subgoal&>(v).Point(), end);
    });

  std::vector<Waypoint*> waypointPath;
  for (auto v : vertexPath) {
    auto subgoal = static_cast<const Subgoal*>(v);
    waypointPath.push_back(new Waypoint(subgoal, tileSize));
  }

  for (auto subgoal : endSubgoals) {
    subgoal->RemoveAdjacency();
  }

  return waypointPath;
}

SubgoalPathFinder::SearchNode::SearchNode(SearchNode *parent,
                                          Subgoal *subgoal,
                                          const Vector2i &goal) :
    parent(parent), subgoal(subgoal) {
  Vector2f myPoint = Util::ToVector2f(GetSubgoal()->Point());
  if (parent == nullptr) {
    dist = 0;
  } else {
    Vector2f parentPoint = Util::ToVector2f(parent->GetSubgoal()->Point());
    dist = parent->dist + Util::Distance(parentPoint, myPoint);
  }
  minDist = dist + Util::Distance(myPoint, Util::ToVector2f(goal));
}