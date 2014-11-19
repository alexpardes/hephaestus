#include "stdafx.h"
#include "MoveAbility.h"
#include "Util.h"
#include "GameUnit.h"
#include "GameState.h"
#include "DirectedSegment.h"
#include "Rectangle.h"
#include "Waypoint.h"
#include <PathFinding/PathFinder.h>

const float kPathingEpsilon = 1.f;

MoveAbility::MoveAbility(std::shared_ptr<GameUnit> owner,
                         PathFinder *pathfinder,
                         GameState *gameState,
                         float speed) {
  this->owner = owner;
  this->pathfinder = pathfinder;
  this->gameState = gameState;
  this->maxSpeed = speed;
  destination = nullptr;
  acceleration = 0.75f * speed;
}

void MoveAbility::SetDestination(const Vector2f &destination) {
  Vector2f start = owner->Position();
  std::vector<const Waypoint*> gridPath = pathfinder->GetPath(start, destination);
  path.clear();

  for (auto waypoint : gridPath) {
    path.push_back(waypoint);
  }

  if (path.size()) {
    path.pop_back(); // Don't want to go to the center of the destination tile.
    this->destination = new Vector2f(destination);
  }
}

void MoveAbility::Execute() {
  while (!path.empty() &&
      path.front()->IsReached(owner->Position(),
          owner->Attributes().CollisionRadius())) {
    path.pop_front();
  }

  Vector2f *currentGoal = nullptr;
  if (!path.empty()) {
    currentGoal = new Vector2f(path.front()
        ->Position(owner->Attributes().CollisionRadius() + kPathingEpsilon));
  } else if (destination) {
    currentGoal = destination;
  }

  if (currentGoal) {
    Vector2f displacement = *currentGoal - owner->Position();
    Util::Limit(displacement, maxSpeed);

    if (Util::Length(displacement) != 0)
      owner->SetRotation(Util::FindAngle(displacement));

    Vector2f newPosition = owner->Position() + displacement;
    //auto collision = gameState->TestWallCollision(owner->Position(), newPosition, owner->Attributes().CollisionRadius() - kPathingEpsilon);
    //if (collision.point != newPosition) {
    //  Util::Limit(displacement, kPathingEpsilon);
    //  newPosition = collision.point - displacement;
    //  destination = nullptr;
    //}

    gameState->MoveUnit(owner->Id(), newPosition);
  }

  //MoveDynamic(currentGoal);
}

void MoveAbility::MoveDynamic(const Vector2f *currentGoal) {
  if (currentGoal) {
    Vector2f direction = Seek(*currentGoal) + /*0.001f * Avoid() +*/ Separate();
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

Vector2f MoveAbility::Separate() const {
  Vector2f direction = SeparateFromUnits() + SeparateFromWalls();
  //Util::Limit(direction, acceleration);
  return direction;
}

Vector2f MoveAbility::SeparateFromWalls() const {
  float radius = 3*owner->Attributes().CollisionRadius();
  Vector2f offset(radius, radius);
  Vector2f topLeft = owner->Position() - offset;
  Vector2f bottomRight = owner->Position() + offset;

  Vector2f desiredDirection;
  std::vector<Rect> walls =
      gameState->GetWallsInRectangle(Rect(topLeft, bottomRight));
  for (Rect wall : walls) {   
    Vector2f direction = owner->Position()
        - wall.NearestPoint(owner->Position());
    float distance = Util::Length(direction) - owner->Attributes().CollisionRadius();
    ScaleForce(direction, distance);

    desiredDirection += direction;
  }

  //Util::Limit(desiredDirection, acceleration);
  return desiredDirection;
}

Vector2f MoveAbility::SeparateFromUnits() const {
  std::vector<std::shared_ptr<GameUnit>> nearbyUnits =
    gameState->GetUnitsInCircle(owner->Position(),
    3*owner->Attributes().CollisionRadius() + maxSpeed);

  Vector2f desiredDirection;

  for (std::shared_ptr<GameUnit> unit : nearbyUnits) {
    if (unit != owner) {
      Vector2f direction = owner->Position() - unit->Position();
      float totalRadius = owner->Attributes().CollisionRadius()
          + unit->Attributes().CollisionRadius();
      float distance = Util::Length(direction) - totalRadius;
      ScaleForce(direction, distance);
      desiredDirection += direction;
    }
  }

  //Util::Limit(desiredDirection, acceleration);
  return desiredDirection;
}

Vector2f MoveAbility::Avoid() const {
  Vector2f force = AvoidUnits() + AvoidWalls();
  //Util::Limit(force, acceleration);
  return force;
}

Vector2f MoveAbility::AvoidUnits() const {
  std::vector<std::shared_ptr<GameUnit>> unitsInFront = gameState->GetUnitsInCircle(
    owner->Position() + velocity, 2 * Util::Length(velocity)
    + owner->Attributes().CollisionRadius());

  DirectedSegment viewSegment(owner->Position(),
    owner->Position() + 2.f*velocity);

  Vector2f newDirection;
  for (std::shared_ptr<GameUnit> unit : unitsInFront) {
    if (unit != owner) {
      float totalRadius = owner->Attributes().CollisionRadius()
        + unit->Attributes().CollisionRadius();
      Vector2f *collisionPoint =
        viewSegment.CollisionPoint(unit->Position(), totalRadius);

      if (collisionPoint) {
        delete collisionPoint;
        newDirection += AvoidForce(unit->Position());
      }
    }
  }
  //Util::Limit(newDirection, acceleration);
  return newDirection;
}

Vector2f MoveAbility::AvoidWalls() const {
  DirectedSegment viewSegment(owner->Position(),
    owner->Position() + 2.f*velocity);
  Rect boundingRect = Util::BoundingRectangle(viewSegment);
  float radius = owner->Attributes().CollisionRadius()
      + Util::Length(velocity);
  Vector2f offset(radius, radius);
  boundingRect.topLeft -= offset;
  boundingRect.bottomRight += offset;

  Vector2f newDirection;

  std::vector<Rect> walls = gameState->GetWallsInRectangle(boundingRect);
  for (Rect wall : walls) {
    Vector2f *collisionPoint = viewSegment.CollisionPoint(wall,
        owner->Attributes().CollisionRadius());

    if (collisionPoint) {
      delete collisionPoint;
      newDirection += AvoidForce(wall.NearestPoint(owner->Position()));
    }
  }

  //Util::Limit(newDirection, acceleration);
  return newDirection;
}

Vector2f MoveAbility::AvoidForce(const Vector2f &point, int degree) const {
  Vector2f direction = point - owner->Position();
  Vector2f desiredDirection = Util::Perpendicular(direction);
  if (Util::Laterality(desiredDirection, direction)
    != Util::Laterality(velocity, direction)) {
      desiredDirection *= -1.f;        
  }
  float weight = Util::Dot(Util::Normalized(direction),
    Util::Normalized(velocity)) / std::powf(Util::Length(direction), float(degree));
  Util::Resize(desiredDirection, weight);

  return desiredDirection;
}

Vector2f MoveAbility::HandleCollisions() {
  Vector2f newPosition = owner->Position() + velocity;
  DirectedSegment movement(owner->Position(), owner->Position() + velocity);
  return gameState->TestCollision(owner, newPosition).point;
}

Vector2f MoveAbility::Seek(const Vector2f &target) {
  Vector2f direction = target - owner->Position();
  Util::Limit(direction, maxSpeed);
  //Util::Normalize(direction);
  return direction;
}

void MoveAbility::Stop() {
  velocity = Vector2f(0, 0);
  //ApplyForce(-velocity, false);
}

void MoveAbility::ApplyForce(Vector2f &force, bool useMaxAcceleration) {
  float magnitude = Util::Length(force);
  if (magnitude > 0 && (useMaxAcceleration || magnitude > acceleration)) {
    Util::Resize(force, acceleration);
  }
  velocity += force;
  if (Util::Length(velocity) > maxSpeed) {
    Util::Resize(velocity, maxSpeed);
  }
}

void MoveAbility::ScaleForce(Vector2f &force, float distance) const {
  if (distance > 0) {
    Util::Resize(force, 100*acceleration / (distance*distance));
  } else {
    Util::Resize(force, 1000*acceleration);
  }
}

bool MoveAbility::DestinationReached() const {
  return !destination && path.empty() || destination && Util::Distance(owner->Position(), *destination) < 5;
}

void MoveAbility::SetPath(const std::deque<const Waypoint*> &path) {
  this->path = path;
  destination = nullptr;
}