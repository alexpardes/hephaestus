#include "stdafx.h"
#include <PathFinding/Subgoal.h>
#include "Waypoint.h"
#include "Util.h"

Waypoint::Waypoint(const Subgoal *subgoal, const Vector2f &tileSize) {  
  position.x = subgoal->Point().x * tileSize.x;
  position.y = subgoal->Point().y * tileSize.y;
  if (subgoal->Direction().x < 0) position.x += tileSize.x;
  if (subgoal->Direction().y < 0) position.y += tileSize.y;

  direction = Util::ToVector2f(subgoal->Direction());
}

Waypoint::Waypoint(const Vector2f &destination) {
  position = destination;
}

Vector2f Waypoint::Position(float radius) const {  
  return position + radius * direction;
}

bool Waypoint::IsReached(const Vector2f &position, float radius) const {
  bool reachedX = position.x * direction.x >= Position(radius).x * direction.x;
  bool reachedY = position.y * direction.y >= Position(radius).y * direction.y;
  return reachedX && reachedY;
}