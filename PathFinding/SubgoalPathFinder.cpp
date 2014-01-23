#include "SubgoalPathFinder.h"
#include <Hephaestus/Util.h>
#include <boost/unordered_set.hpp>
#include "GridRegion.h"

SubgoalPathFinder::SubgoalPathFinder(
    const PathingGrid *grid,
    const std::vector<const Vector2i> &subgoalPoints,
    const std::vector<const std::vector<int>> &adjacencyLists) : grid(grid) {
  std::vector<const Vector2i>::const_iterator it = subgoalPoints.begin();
  while (it != subgoalPoints.end()) {
    subgoals.push_back(new Subgoal(*it++));
  }

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

bool SubgoalPathFinder::SubgoalComparer::operator()(
    SubgoalPathFinder::Subgoal *a,
    SubgoalPathFinder::Subgoal *b) {
  float distance1 = PathingGrid::OctileDistance(a->Point(), point);
  float distance2 = PathingGrid::OctileDistance(b->Point(), point);
  return distance1 < distance2;
}

std::vector<SubgoalPathFinder::Subgoal*> SubgoalPathFinder::GetDirectSubgoals(
    const Vector2i &p) {

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

// Adds the start and end points to the subgoal graph, and then A* searches through it.
// Returns an empty path if no path exists. Returns the endpoint if start and
// end are directly connected.
std::vector<Vector2i> SubgoalPathFinder::GetPath(const Vector2i &start,
                                                 const Vector2i &end) {

  boost::unordered_set<Vector2i, Vector2iHash> endGoals;
  boost::unordered_set<Vector2i, Vector2iHash> visitedList;

  std::priority_queue<SearchNode*,
                      std::vector<SearchNode*>,
                      SearchNodeComparer> openList;

  // We search from end to start so that when we trace back the path, it is
  // already in the correct order.
  // TODO: fix memory leaks
  std::vector<Subgoal*> startSubgoals = GetDirectSubgoals(end);
  Subgoal *startGoal = new Subgoal(end);
  SearchNode *startNode = new SearchNode(NULL, startGoal, start);
  for (int i = 0; i < startSubgoals.size(); ++i) {
    openList.push(new SearchNode(startNode, startSubgoals.at(i), start));
  }

  std::vector<Subgoal*> endSubgoals = GetDirectSubgoals(start);
  for (int i = 0; i < endSubgoals.size(); ++i) { 
    endGoals.insert(endSubgoals.at(i)->Point());
  }

  SearchNode *endNode = NULL;
  while (!openList.empty()) {
    SearchNode *node = openList.top();
    openList.pop();
    Subgoal *subgoal = node->GetSubgoal();

    if (visitedList.count(subgoal->Point())) {
      continue;
    }
    visitedList.insert(subgoal->Point());

    if (endGoals.count(subgoal->Point())) { // We've found a path.
      endNode = node;
      break;
    }

    // Adds unvisited neighboring subgoals to the open list.
    for (int i = 0; i < subgoal->AdjacencyList().size(); ++i) {
      Subgoal *neighbor = subgoal->AdjacencyList().at(i);
      if (!visitedList.count(neighbor->Point())) {
        openList.push(new SearchNode(node, neighbor, start));
      }
    }
  }

  std::vector<Vector2i> path;

  if (AreDirectlyConnected(start, end) && (!endNode ||
      endNode->MinDist() > PathingGrid::OctileDistance(start, end))) {
    path.push_back(end);
  } else {
    while (endNode) {
      path.push_back(endNode->GetSubgoal()->Point());
      SearchNode *parent = endNode->Parent();
      endNode = parent;
    }
  }

  delete startGoal;
  return path;
}

SubgoalPathFinder::SearchNode::SearchNode(SearchNode *parent,
                                          Subgoal *subgoal,
                                          const Vector2i &goal) :
    parent(parent), subgoal(subgoal) {
  Vector2f myPoint = Util::GetVector2f(GetSubgoal()->Point());
  if (parent == NULL) {
    dist = 0;
  } else {
    Vector2f parentPoint = Util::GetVector2f(parent->GetSubgoal()->Point());
    dist = parent->dist + Util::Distance(parentPoint, myPoint);
  }
  minDist = dist + Util::Distance(myPoint, Util::GetVector2f(goal));
}