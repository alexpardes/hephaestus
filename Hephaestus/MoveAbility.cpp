#include "stdafx.h"
#include "MoveAbility.h"
#include "Util.h"
#include "GameUnit.h"
#include "GameState.h"
#include "DirectedSegment.h"
#include "Rectangle.h"
#include <PathFinding/PathFinder.h>
#include <PathFinding/Subgoal.h>

MoveAbility::MoveAbility(GameUnit *owner,
                         PathFinder *pathfinder,
                         GameState *gameState,
                         float speed) {
  this->owner = owner;
  this->pathfinder = pathfinder;
  this->gameState = gameState;
  this->maxSpeed = speed;
  acceleration = 0.75 * speed;
}

void MoveAbility::SetDestination(const Vector2f &destination) {
  Vector2i start = gameState->GetTile(owner->Id());
  Vector2i end = gameState->GetTile(destination);
  std::vector<Subgoal*> gridPath = pathfinder->GetPath(start, end);
  path.clear();

  for (size_t i = 0; i < int(gridPath.size()) ; ++i) {
    Subgoal *subgoal = gridPath.at(i);
    path.push_back(subgoal);
  }

  if (path.size()) {
    path.pop_back(); // Don't want to go to the center of the destination tile.
    this->destination = new Vector2f(destination);
  }
}

void MoveAbility::Execute() {
  while (!path.empty() &&
      path.front()->IsPast(gameState->GetTile(owner->position()))) {
    path.pop_front();
  }

  Vector2f *waypoint;
  if (!path.empty()) {
    waypoint = new Vector2f(gameState->GetLocation(path.front()->Point()));
  } else if (destination) {
    waypoint = destination;
  }

  if (waypoint) {
    Vector2f direction = Seek(*waypoint) + Avoid() + Separate();
    Util::Limit(direction, maxSpeed);
    ApplyForce(direction - velocity, false);
  } else {
    Stop();
  }
  
  assert(velocity.x == velocity.x);
  Vector2f newPosition = HandleCollisions();

  gameState->MoveUnit(owner->Id(), newPosition);
  if (Util::Length(velocity) > 0) {
    owner->SetRotation(Util::FindAngle(velocity));
  }
}

Vector2f MoveAbility::Separate() {
  Vector2f direction = SeparateFromUnits() + SeparateFromWalls();
  Util::Limit(direction, maxSpeed);
  return direction;
}

Vector2f MoveAbility::SeparateFromWalls() {
  float radius = 3*owner->Attributes().collision_radius();
  Vector2f offset(radius, radius);
  Vector2f topLeft = owner->position() - offset;
  Vector2f bottomRight = owner->position() + offset;

  Vector2f desiredDirection;
  std::vector<Rect> walls =
      gameState->GetWallsInRectangle(Rect(topLeft, bottomRight));
  for (size_t i = 0; i < walls.size(); ++i) {
    Rect wall = walls.at(i);
    Vector2f direction = owner->position() - wall.Center();
    Util::Normalize(direction);
    desiredDirection += direction;
  }

  return desiredDirection;
}

Vector2f MoveAbility::SeparateFromUnits() {
  std::vector<GameUnit*> nearbyUnits =
    gameState->GetUnitsInCircle(owner->position(),
    3*owner->Attributes().collision_radius() + maxSpeed);

  Vector2f desiredDirection;

  for (size_t i = 0; i < nearbyUnits.size(); ++i) {
    GameUnit *unit = nearbyUnits.at(i);
    if (unit != owner) {
      Vector2f direction = owner->position() - unit->position();
      float totalRadius = owner->Attributes().collision_radius()
          + unit->Attributes().collision_radius();
      float distance = Util::Length(direction) - totalRadius;
      if (distance > 0) {
        Util::Scale(direction, 100*acceleration / (distance*distance));
      } else {
        Util::Scale(direction, FLT_MAX / 100);
      }
      desiredDirection += direction;
    }
  }

  return desiredDirection;
}

Vector2f MoveAbility::Avoid() {
  std::vector<GameUnit*> unitsInFront = gameState->GetUnitsInCircle(
      owner->position() + velocity, 2 * Util::Length(velocity)
      + owner->Attributes().collision_radius());

  DirectedSegment viewSegment(owner->position(),
      owner->position() + 2.f*velocity);

  Vector2f newDirection;
  for (size_t i = 0; i < unitsInFront.size(); ++i) {
    GameUnit *unit = unitsInFront.at(i);
    if (unit != owner) {
      float totalRadius = owner->Attributes().collision_radius()
          + unit->Attributes().collision_radius();
      Vector2f *collisionPoint =
          viewSegment.CollisionPoint(unit->position(), totalRadius);

      if (collisionPoint) {
        delete collisionPoint;
        Vector2f direction = unit->position() - owner->position();
        Vector2f desiredDirection = Util::Perpendicular(direction);
        if (Util::Laterality(desiredDirection, direction)
            != Util::Laterality(velocity, direction)) {
          desiredDirection *= -1.f;        
        }
        float weight = Util::Dot(Util::Normalized(direction),
            Util::Normalized(velocity));
        newDirection += weight*desiredDirection;
      }
    }
  }
  Util::Limit(newDirection, maxSpeed);
  return newDirection;
}

Vector2f MoveAbility::HandleCollisions() {
  Vector2f newPosition(owner->position() + velocity);
  newPosition = HandleUnitCollisions(newPosition);
  newPosition = HandleTerrainCollisions(newPosition);
  return newPosition;
}

Vector2f MoveAbility::HandleTerrainCollisions(const Vector2f &end) {
  DirectedSegment movement(owner->position(), end);
  float movementDist = Util::Length(owner->position() - end);
  if (movementDist != 0) {
    Rect boundingRect = Util::BoundingRectangle(movement);
    float radius = owner->Attributes().collision_radius();
    boundingRect.topLeft -= Vector2f(radius, radius);
    boundingRect.bottomRight += Vector2f(radius, radius);
    std::vector<Rect> walls =
        gameState->GetWallsInRectangle(boundingRect);
    std::vector<Rect>::const_iterator it = walls.begin();
    while (it != walls.end()) {
      Rect wall = *it++;
      Vector2f *collisionPoint = movement.CollisionPoint(wall, radius);
      if (collisionPoint) {
        movement = DirectedSegment(owner->position(), *collisionPoint);
      }
    }
  }

  return movement.End();
}

Vector2f MoveAbility::HandleUnitCollisions(const Vector2f &end) {
  DirectedSegment movement(owner->position(), end);
  float movementDist = Util::Length(owner->position() - end);
  if (movementDist != 0) {
    Vector2f movementCenter = owner->position() + 0.5f*velocity;
    std::vector<GameUnit*> nearbyUnits =
      gameState->GetUnitsInCircle(movementCenter,
      movementDist/2 + owner->Attributes().collision_radius());
    for (size_t i = 0; i < nearbyUnits.size(); ++i) {
      GameUnit *unit = nearbyUnits.at(i);
      if (unit != owner) {
        //float margin = 1.00001f;
        float totalRadius = owner->Attributes().collision_radius()
          + unit->Attributes().collision_radius();
        Vector2f *collisionPoint =
          movement.CollisionPoint(unit->position(), totalRadius);
        if (collisionPoint) {
          float distance = Util::Distance(*collisionPoint, unit->position());
          assert(distance > totalRadius);
          movement = DirectedSegment(owner->position(), *collisionPoint);
          delete collisionPoint;
        }
      }
    }
  }

  std::list<GameUnit*>::const_iterator it = gameState->units().begin();
  while (it != gameState->units().end()) {
    GameUnit *unit = *it++;
    if (unit != owner) {
      float distance = Util::Distance(unit->position(), movement.End());
      float totalRadius = owner->Attributes().collision_radius()
          + unit->Attributes().collision_radius();
      assert(distance > totalRadius);
    }
  }

  return movement.End();
}

Vector2f MoveAbility::Seek(const Vector2f &target) {
  Vector2f direction = target - owner->position();
  Util::Limit(direction, maxSpeed);
  //Util::Normalize(direction);
  return direction;
}

void MoveAbility::Stop() {
  velocity = Vector2f(0, 0);
  //ApplyForce(-velocity, false);
}

void MoveAbility::ApplyForce(Vector2f force, bool useMaxAcceleration) {
  float magnitude = Util::Length(force);
  if (magnitude > 0 && (useMaxAcceleration || magnitude > acceleration)) {
    Util::Scale(force, acceleration);
  }
  velocity += force;
  if (Util::Length(velocity) > maxSpeed) {
    Util::Scale(velocity, maxSpeed);
  }
}