#pragma once
#include <Hephaestus/PathFinder.h>

class SubgoalPathFinder : public PathFinder {
  public:
    SubgoalPathFinder(const PathingGrid *grid,
        const std::vector<const Vector2i> &subgoalPoints,
        const std::vector<const std::vector<int>> &adjacencyLists);

    virtual std::vector<Vector2i> GetPath(const Vector2i &start,
        const Vector2i &end);
    virtual ~SubgoalPathFinder();

    class Subgoal {
      public:
        Subgoal(const Vector2i &point) : point(point) { }
        void AddAdjacency(Subgoal *subgoal) {
          adjacencyList.push_back(subgoal);
        }

        const std::vector<Subgoal*> &AdjacencyList() { return adjacencyList; }
        const Vector2i &Point() { return point; }

    private:
      Vector2i point;
      std::vector<Subgoal*> adjacencyList;      
    };


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
    bool CanMoveInDirection(const Vector2i &p, const Vector2i &direction);
    bool AreDirectlyConnected(const Vector2i &p1, const Vector2i &p2);
    Vector2i GetDirection(const Vector2i &p1, const Vector2i &p2);
    std::vector<Subgoal*> GetDirectSubgoals(const Vector2i &p);
};