#include "SubgoalPathFinder.h"
#include <queue>
#include <unordered_set>
#include <Hephaestus/Vector2.h>
#include <Hephaestus/Util.h>
#include <Hephaestus/Waypoint.h>
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

// Adds the start and end points to the subgoal graph, and then A* searches through it.
// Returns an empty path if no path exists. Returns the endpoint if start and
// end are directly connected.
std::vector<Waypoint*> SubgoalPathFinder::GetPath(const Vector2f &startpoint,
                                                 const Vector2f &endpoint) {

  std::unordered_set<Vector2i, Vector2iHash> endGoals;
  std::unordered_set<Vector2i, Vector2iHash> visitedList;

  // Keeps track of all the nodes created so we can delete them.
  std::vector<SearchNode*> usedNodes;

  std::priority_queue<SearchNode*,
                      std::vector<SearchNode*>,
                      SearchNodeComparer> openList;

  // We search from end to start so that when we trace back the path, it is
  // already in the correct order.
  Vector2i start = GetTile(endpoint);
  Vector2i end = GetTile(startpoint);

  std::vector<Subgoal*> startSubgoals = GetDirectSubgoals(start);
  Subgoal *startGoal = new Subgoal(start, Vector2i(0, 0));

  SearchNode *startNode = new SearchNode(NULL, startGoal, end);
  usedNodes.push_back(startNode);
  for (int i = 0; i < startSubgoals.size(); ++i) {
    SearchNode *node = new SearchNode(startNode, startSubgoals.at(i), end);
    openList.push(node);
    usedNodes.push_back(node);
  }

  std::vector<Subgoal*> endSubgoals = GetDirectSubgoals(end);
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
        SearchNode *neighborNode = new SearchNode(node, neighbor, end);
        openList.push(neighborNode);
        usedNodes.push_back(neighborNode);
      }
    }
  }

  std::vector<Waypoint*> path;

  if (AreDirectlyConnected(end, start) && (!endNode ||
      endNode->MinDist() > PathingGrid::OctileDistance(end, start))) {
    path.push_back(new Waypoint(endpoint));
  } else {
    while (endNode) {
      SearchNode *parent = endNode->Parent();
      if (parent) {
        path.push_back(new Waypoint(endNode->GetSubgoal(), tileSize));
      } else {
        path.push_back(new Waypoint(endpoint));
      }
      endNode = parent;
    }
  }

  delete startGoal;
  for (SearchNode *node : usedNodes) {
    delete node;
  }

  return path;
}

SubgoalPathFinder::SearchNode::SearchNode(SearchNode *parent,
                                          Subgoal *subgoal,
                                          const Vector2i &goal) :
    parent(parent), subgoal(subgoal) {
  Vector2f myPoint = Util::ToVector2f(GetSubgoal()->Point());
  if (parent == NULL) {
    dist = 0;
  } else {
    Vector2f parentPoint = Util::ToVector2f(parent->GetSubgoal()->Point());
    dist = parent->dist + Util::Distance(parentPoint, myPoint);
  }
  minDist = dist + Util::Distance(myPoint, Util::ToVector2f(goal));
}