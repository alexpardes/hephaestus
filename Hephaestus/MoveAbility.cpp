#include "stdafx.h"
#include "MoveAbility.h"
#include "Util.h"
#include "GameUnit.h"
#include "GameState.h"
#include "PathFinder.h"

MoveAbility::MoveAbility(GameUnit *owner,
                         PathFinder *pathfinder,
                         GameState *gameState,
                         float speed) {
  this->owner = owner;
  this->pathfinder = pathfinder;
  this->gameState = gameState;
  this->maxSpeed = speed;
  acceleration = speed;
}

void MoveAbility::SetDestination(const Vector2f &destination) {
  Vector2i start = gameState->GetTile(owner->Id());
  Vector2i end = gameState->GetTile(destination);
  std::vector<Vector2i> gridPath = pathfinder->GetPath(start, end);
  path.clear();

  for (int i = 0; i < int(gridPath.size()) ; ++i) {
    path.push_back(gameState->GetLocation(gridPath.at(i)));
  }

  // Adds the destination to the path separately, so that we don't move to the
  // center of the destination's tile.
  if (path.size()) {
    path.pop_back();
    path.push_back(destination);
  }
}

void MoveAbility::Execute() {
  //float movementDistance = speed;
  //Vector2f newPosition = owner->position();
  //Vector2f direction;

  //while (movementDistance > 0.0 && !path.empty()) {
  //  float legDistance = Util::Distance(newPosition, path.front());
  //  if (legDistance <= movementDistance) {
  //    newPosition = path.front();
  //    movementDistance -= legDistance;
  //    path.pop_front();
  //  } else {
  //    direction = Util::GetNormalized(path.front() - newPosition);
  //    newPosition += movementDistance * direction;
  //    movementDistance = 0.0;
  //  }
  //}
  //owner->SetPosition(newPosition);
  //if (Util::Length2(direction) > 0) {
  //  owner->SetRotation(Util::FindAngle(direction));
  //}

  while (!path.empty()) {
    if (Util::Distance(owner->position(), path.front()) < 15) {
      path.pop_front();
    } else {
      Seek(path.front());
      break;
    }
  }

  if (path.empty()) {
    Stop();
  }

  gameState->MoveUnit(owner->Id(), owner->position() + velocity);
  if (Util::Length(velocity) > 0) {
    owner->SetRotation(Util::FindAngle(velocity));
  }
}

void MoveAbility::Seek(const Vector2f &target) {
  Vector2f direction = target - owner->position();
  Vector2f force = direction - velocity;
  ApplyForce(force, true);
}

void MoveAbility::Stop() {
  ApplyForce(-velocity, false);
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