#include "stdafx.h"
#include "AttackAbility.h"
#include "GameState.h"
#include "Util.h"
#include "MoveAbility.h"
#include "Circle.h"

AttackAbility::AttackAbility(std::shared_ptr<GameUnit> owner,
                             GameState *gameState,
                             float damage,
                             float dispersion,
                             float speed,
                             float projectileSpeed,
                             float range) :
    owner(owner), gameState(gameState), damage(damage), dispersion(dispersion),
    speed(speed), projectileSpeed(projectileSpeed), range(range), loadTime(0) {

  useTargetPoint = false;
  moveAbility = static_cast<MoveAbility*>(owner->GetAbility("Move"));
  movementEnabled = moveAbility != nullptr;
}

void AttackAbility::Execute() {
  auto unit = gameState->GetUnit(target);
  if (unit && CanAttack(target))
    Attack(*unit);
}

void AttackAbility::ExecutePassive() {
  auto unit = gameState->GetUnit(target);
  if (unit && CanAttack(target)) {
    Attack(*unit);
  } else {
    float desiredRotation = owner->Rotation();
    if (useTargetPoint) {
      auto v = targetPoint - owner->Position();
      if (Util::Length(v) > 0)
        desiredRotation = Util::FindAngle(v);
    }

    Rotate(desiredRotation, useTargetPoint);
  }
  loadTime -= speed;
  loadTime = std::max(loadTime, 0.f);
}

void AttackAbility::SetTargetPoint(const Vector2f &point) {
  targetPoint = point;
  useTargetPoint = true;
}

void AttackAbility::Rotate(float angle, bool isStrafing) const {
  auto relativeAngle = Util::Angle(angle - owner->Rotation(), -M_PI);
  if (isStrafing) {
    moveAbility->SetStrafeDirection(angle);
  } else {
    moveAbility->StopStrafing();
  }

  auto maxAngle = Util::Radians(100);
  auto rotationSpeed = Util::Radians(30);
  relativeAngle = Util::Limit(relativeAngle, maxAngle);
  auto currentRotation = Util::Angle(owner->RelativeTorsoRotation(), -M_PI);
  auto rotationChange = Util::Limit(relativeAngle - currentRotation, rotationSpeed);
  auto newRotation = currentRotation + rotationChange;
  owner->SetRelativeTorsoRotation(newRotation);
  auto handSwitchAngle = Util::Radians(20);
  if (Util::IsBetweenAngles(newRotation, handSwitchAngle, M_PI)) {
    owner->SetFacing(kRight);
  } else if (Util::IsBetweenAngles(newRotation, M_PI, -handSwitchAngle)) {
    owner->SetFacing(kLeft);
  }
}

bool AttackAbility::CanAttack(UnitId target) const {
  auto unit = gameState->GetUnit(target);
  if (!unit || !IsInRange(*unit))
    return false;

  return owner->SightMap().VisibleSize(unit->SegmentFromUnit(owner->Position())) > 0.05f * dispersion;
}

void AttackAbility::Attack(const GameUnit &unit) {
  auto velocity = unit.Position() - unit.PreviousPosition();
  auto distance = Util::Distance(unit.Position(), owner->Position());
  auto predictedTime = distance / projectileSpeed;
  auto predictedLocation = unit.Position() + predictedTime * velocity;
  Circle circle(predictedLocation, unit.Attributes().CollisionRadius());
  auto widestPoints = circle.WidestPoints(owner->Position());
  auto segment = LineSegment(widestPoints.first, widestPoints.second);

  auto targetSegment = owner->SightMap().LargestVisibleSubsegment(segment);
  if (targetSegment.Length() > 0) {
    auto targetPoint = (targetSegment.p1 + targetSegment.p2) / 2.f;
    float targetAngle = Util::FindAngle(targetPoint - owner->Position());
    Rotate(targetAngle, true);
    if (owner->TorsoRotation() != targetAngle)
      return;

    auto instabilityDispersion = (1 - owner->Stability()) * Util::Radians(45);
    if (loadTime <= 0.f) {
      gameState->AddProjectile(
        owner,
        owner->Position(),
        targetAngle,
        damage,
        dispersion + instabilityDispersion,
        projectileSpeed);

      loadTime = 10.f;
    }
  }
}

bool AttackAbility::IsInRange(const GameUnit &unit) const {
  bool result = false;
  if (Util::Distance(unit.Position(), owner->Position()) < range) {
    result = true;
  }
  return result;
}

bool AttackAbility::IsInRange() const {
  return IsInRange(*gameState->GetUnit(target));
}

const std::shared_ptr<GameUnit> AttackAbility::Target() const {
  return gameState->GetUnit(target);
}