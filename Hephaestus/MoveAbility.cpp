#include "stdafx.h"
#include "MoveAbility.h"
#include "AutoAttackAbility.h"
#include "Util.h"
#include "GameUnit.h"
#include "GameState.h"
#include "DirectedSegment.h"
#include "Rectangle.h"
#include <PathFinding/PathFinder.h>
#include <PathFinding/SpatialGraph.h>
#include <PathFinding/SpatialVertex.h>

const float kPathingEpsilon = 1.f;

MoveAbility::MoveAbility(std::shared_ptr<GameUnit> owner,
                         GameState &gameState) : gameState(gameState) {
  this->owner = owner;
  isStrafing = false;
  angularVelocity = 0.f;
}

void MoveAbility::SetDestination(const Vector2f &destination) {
  auto start = gameState.PathingGraph().MakeVertex(StoppingPoint());
  path = PathFinder::Path(start.get(), gameState.DilatedWalls(), destination);
}

/* Returns the maximum speed a unit can have while able to turn from a velocity
 * of v1 to a velocity in the direction of v2 with equal or lesser speed than
 * endSpeed. */
float CorneringSpeed(const Vector2f &v1, const Vector2f &v2, float acceleration, float endSpeed) {
  auto angle = Util::FindShortestAngle(v1, v2);
  if (angle >= M_PI / 2) {
    // The closest legal velocity is 0.
    return acceleration;
  }

  if (angle == 0)
    return endSpeed;

  auto a2 = acceleration * acceleration;
  if (endSpeed >= acceleration / std::tanf(angle)) {
    // The closest point on the ray of v2 is a legal velocity.
    return acceleration / std::sin(angle);
  }

  // In this case the closest legal velocity is v2.
  auto endSpeed2 = endSpeed * endSpeed;
  auto cos = std::cosf(angle);
  auto radical = std::sqrtf(1 + (a2 - endSpeed2) / (endSpeed2 * cos * cos));
  return endSpeed * cos * (1 + radical);
}

float CorneringSpeed(const Vector2f &v1, const Vector2f &v2, float acceleration) {
  return CorneringSpeed(v1, v2, acceleration, 1e10);
}

/* Returns the maximum speed a unit traveling with velocity v1 can have after
 * turning to move along v2. */
float SpeedAfterTurn(const Vector2f &v1, const Vector2f &v2, float acceleration) {
  auto initialSpeed = Util::Length(v1);
  auto corneringSpeed = CorneringSpeed(v1, v2, acceleration);

  if (initialSpeed > corneringSpeed) {
    assert(false);
    return corneringSpeed;
  }

  auto minAccelerationSquared = std::powf(acceleration * initialSpeed / corneringSpeed, 2);
  auto minSpeed = std::sqrtf(initialSpeed * initialSpeed - minAccelerationSquared);
  auto extraSpeed = std::sqrtf(acceleration * acceleration - minAccelerationSquared);
  return minSpeed + extraSpeed;
}

/* Returns the maximum speed a unit can travel this turn while able to
 * decelerate to endSpeed the turn before traveling more than  distance. */
float MaxSpeed(float distance, float endSpeed, float acceleration) {
  distance -= endSpeed;
  if (distance < endSpeed)
    return endSpeed;

  auto tEnd = endSpeed / acceleration;
  auto radical = std::sqrtf(1 + 4*tEnd*(tEnd - 1) + 8*distance / acceleration);
  auto t = std::ceilf((radical - 1) / 2 - tEnd);
  assert (t >= 1);
   
  float maxSpeed = std::min((distance)/t + acceleration*(t - 1)/2, endSpeed + acceleration * t);
  assert(maxSpeed >= endSpeed);

  return maxSpeed;
}

float MoveAbility::MaxSpeedAtLegStart(int legIndex) const {
  Vector2f startPoint;
  Vector2f endPoint = path[legIndex];
  if (legIndex == 0) {
    startPoint = owner->Position();
  } else {
    startPoint = path[legIndex - 1];
  }

  auto distance = Util::Distance(startPoint, endPoint);
  if (legIndex == path.size() - 1)
    return MaxSpeed(distance, 0.f, MaxAcceleration());

  auto direction = endPoint - startPoint;
  auto nextDirection = path[legIndex + 1] - endPoint;
  auto maxSpeedAfterTurn = MaxSpeedAtLegStart(legIndex + 1);

  auto endSpeed = CorneringSpeed(direction, nextDirection, MaxAcceleration(), maxSpeedAfterTurn);

  auto maxSpeed = MaxSpeed(distance, endSpeed, MaxAcceleration());

  if (maxSpeed < distance) {
    auto nextMaxSpeed = MaxSpeed(distance - maxSpeed, endSpeed, MaxAcceleration());
    assert(nextMaxSpeed >= 0.99f * maxSpeed - MaxAcceleration());
  }
  return maxSpeed;
}

float MoveAbility::MaxAcceleration() const {
  return owner->Attributes().Speed() / 3.f;
}

void MoveAbility::Execute() {
  while (!path.empty() && path.front() == owner->Position()) {
    path.pop_front();
  }

  if (path.empty())
    return;

  auto direction = path.front() - owner->Position();
  auto maxAcceleration = MaxAcceleration();
  auto distanceToGoal = Util::Length(direction);

  Vector2f nextDirection(0, 0);
  if (path.size() > 1) {
    nextDirection = path[1] - path[0];
  }

  auto maxSpeed = MaxSpeedAtLegStart(0);
  maxSpeed = std::min(maxSpeed, TopSpeed(direction));  

  auto currentSpeed = Util::Length(velocity);

  if (distanceToGoal < maxSpeed - 0.01f) {
    //assert(currentSpeed <= CorneringSpeed(direction, nextDirection, MaxAcceleration()));

    gameState.MoveUnit(owner->Id(), path.front());
    path.pop_front();
    auto speedAfterTurn = SpeedAfterTurn(velocity, nextDirection, maxAcceleration);
    speedAfterTurn = std::min(speedAfterTurn, TopSpeed(nextDirection));
    velocity = Util::Resized(nextDirection, speedAfterTurn);

    auto remainingMovement = Util::Length(velocity) - distanceToGoal;
    if (remainingMovement > 0) {
      auto newPosition = owner->Position() + Util::Resized(velocity, remainingMovement);
      gameState.MoveUnit(owner->Id(), newPosition);
    }
      
    return;
  }

  auto desiredVelocity = Util::Resized(direction, 0.99f * maxSpeed);
  auto desiredSpeed = Util::Length(desiredVelocity);
  //assert(desiredSpeed >= currentSpeed - maxAcceleration);
  auto acceleration = desiredVelocity - velocity;

  Util::Limit(acceleration, maxAcceleration);
  velocity += acceleration;

  Vector2f newPosition = owner->Position() + velocity;
  gameState.MoveUnit(owner->Id(), newPosition);
}

float MoveAbility::TopSpeed(const Vector2f &v) const {
  auto movementAngle = Util::FindAngle(v);
  auto angle = Util::AngleBetween(movementAngle, owner->Rotation());

  auto forwardSpeed = owner->Attributes().Speed();
  auto sidewaysSpeed = 0.5f * forwardSpeed;
  auto backwardSpeed = 0.5f * forwardSpeed;

  float linearSpeed;
  if (angle > M_PI / 2) {
    linearSpeed = backwardSpeed;
    angle = M_PI - angle;
  } else {
    linearSpeed = forwardSpeed;
  }
    
  auto result = linearSpeed * std::cosf(angle) + sidewaysSpeed * std::sinf(angle);
  return result;
}

void MoveAbility::ExecutePassive() {
  if (DestinationReached()) {
    auto acceleration = Util::Limited(-velocity, MaxAcceleration());
    velocity += acceleration;
    gameState.MoveUnit(owner->Id(), owner->Position() + velocity);
  }

  if (Util::Length(velocity) > 1e-3f) {
    auto desiredRotation = Util::FindAngle(velocity);
    bool goBackward = isStrafing && Util::AngleBetween(strafeDirection, desiredRotation) > M_PI / 2;
    if (goBackward)
      desiredRotation = desiredRotation + M_PI;

    Rotate(desiredRotation);
  } else {
    if (isStrafing)
      Rotate(strafeDirection);
  }  

  static_cast<AutoAttackAbility*>(owner->GetAbility("AutoAttack"))->Execute();
}

void MoveAbility::Rotate(float angle) {
  auto rotationSpeed = Util::Radians(30);
  auto currentRotation = owner->Rotation();
  if (currentRotation == angle) {
    angularVelocity = 0;
    return;
  }

  auto angleCw = Util::AngleCw(currentRotation, angle);
  auto angleCcw = Util::AngleCcw(currentRotation, angle);
  bool turnCcw;
  if (isStrafing && Util::AngleBetween(strafeDirection, currentRotation) > M_PI / 2) {
    turnCcw = Util::AngleCcw(currentRotation, strafeDirection) < Util::AngleCw(currentRotation, strafeDirection);
  } else {
    turnCcw = angleCcw < angleCw;
  }

  auto desiredDelta = turnCcw ? angleCcw : -angleCw;
  angularVelocity = Util::Limit(desiredDelta, rotationSpeed);
  auto newRotation = currentRotation + angularVelocity;
  owner->SetRotation(newRotation);
}

bool MoveAbility::DestinationReached() const {
  return path.empty();
}

void MoveAbility::SetPath(const std::deque<const Vector2f> &path) {
  this->path = path;
}

void MoveAbility::SetStrafeDirection(float angle) {
  isStrafing = true;
  strafeDirection = angle;
}

void MoveAbility::StopStrafing() {
  isStrafing = false;
}

Vector2f MoveAbility::StoppingPoint() const {
  auto speed = Util::Length(velocity) - MaxAcceleration();
  auto t = std::ceilf(speed / MaxAcceleration());
  auto distance = speed * t - MaxAcceleration() * t * (t - 1) / 2;
  return owner->Position() + Util::Resized(velocity, distance);
}

void MoveAbility::Stop() {
  path.clear();
}

float MoveAbility::AngularVelocity() const {
  return angularVelocity;
}