#pragma once
#include "PathFinder.h"

class Subgoal;

class SubgoalPathFinder : public PathFinder {
  public:
    SubgoalPathFinder(const PathingGrid *grid,
        const Vector2f &tileSize,
        const std::vector<Subgoal*> &subgoals,
        const std::vector<const std::vector<int>> &adjacencyLists);

    virtual std::vector<Waypoint*> GetPath(const Vector2f &startpoint,
        const Vector2f &endpoint);
    virtual const PathingGrid *GetPathingGrid() const { return grid; }
    virtual ~SubgoalPathFinder();
    virtual void SetTileSize(const Vector2f& newSize) { tileSize = newSize; }


    class SearchNode {
      public:
        SearchNode(SearchNode *parent, Subgoal *subgoal, const Vector2i &goal);
        SearchNode *Parent() const { return parent; }
        Subgoal *GetSubgoal() { return subgoal; }
        float MinDist() { return minDist; }

      private:
        SearchNode *parent;
        Subgoal *subgoal;
        float minDist;
        float dist;
    };


  private:
    class SubgoalComparer {
      public:
        SubgoalComparer() { }
        SubgoalComparer(const Vector2i point) : point(point) { }
        // Returns true if a is closer to point than b.
        bool operator()(Subgoal *a, Subgoal *b);

      private:
        Vector2i point;
    };

    class SearchNodeComparer {
      public:
        bool operator()(SearchNode *a, SearchNode *b) {
          return a->MinDist() > b->MinDist();
        }
    };

    const PathingGrid *grid;
    std::vector<Subgoal*> subgoals;   
    Vector2f tileSize;
    bool CanMoveInDirection(const Vector2i &p, const Vector2i &direction);
    bool AreDirectlyConnected(const Vector2i &p1, const Vector2i &p2);
    Vector2i GetDirection(const Vector2i &p1, const Vector2i &p2);
    std::vector<Subgoal*> GetDirectSubgoals(const Vector2i &p);
    Vector2i GetTile(const Vector2f &position);
};