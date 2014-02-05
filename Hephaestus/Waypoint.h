#pragma once
#include "Vector2.h"

class Subgoal;

class Waypoint {
  public:
    Waypoint(const Subgoal *subgoal, const Vector2f &tileSize);
    Waypoint(const Vector2f &destination);
    Vector2f Position(float radius) const;
    bool IsReached(const Vector2f &position, float radius) const;

  private:
    Vector2f position, direction;    
};