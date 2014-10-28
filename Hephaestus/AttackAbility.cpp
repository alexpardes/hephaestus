#include "stdafx.h"
#include "AttackAbility.h"
#include "GameState.h"
#include "Util.h"
#include "MoveAbility.h"
#include "Circle.h"
#include <Scripting/LuaUtilities.h>

AttackAbility::AttackAbility(std::shared_ptr<GameUnit> owner,
                             GameState *gameState,
                             float damage,
                             float speed,
                             float range) :
    owner(owner), gameState(gameState), damage(damage), speed(speed),
    range(range), loadTime(0) {

  moveAbility = dynamic_cast<MoveAbility*>(owner->GetAbility("Move"));
  movementEnabled = moveAbility != nullptr;
  rightAttackOffset = Vector2f(0, 0.6f * owner->Attributes().CollisionRadius());
  leftAttackOffset = -rightAttackOffset;
}

Vector2f AttackAbility::ApplyUnitTransform(const Vector2f& v) const {
  Vector2f result = v;
  Util::Rotate(result, owner->Rotation());
  result += owner->Position();
  return result;
}

void AttackAbility::ChangeAttackPoint() {
  usingLeftAttackPoint = !usingLeftAttackPoint;
}

Vector2f AttackAbility::AttackPoint() const {
  if (usingLeftAttackPoint) {
    return LeftAttackPoint();
  } else {
    return RightAttackPoint();
  }
}

Vector2f AttackAbility::LeftAttackPoint() const {
  return ApplyUnitTransform(leftAttackOffset);
}

Vector2f AttackAbility::RightAttackPoint() const {
  return ApplyUnitTransform(rightAttackOffset);
}

// TODO: clean up this method.
void AttackAbility::Execute() {
  auto unit = gameState->GetUnit(target);
  if (unit) {
    std::vector<Vector2f> unobstructedPoints;
    if (CanAttack(target, &unobstructedPoints)) {
      Attack(unit, unobstructedPoints);
    } else if (movementEnabled) {
      moveAbility->SetDestination(unit->Position());
      moveAbility->Execute();
    }
  }
  loadTime -= speed;
}

bool AttackAbility::CanAttack(UnitId target,
                              std::vector<Vector2f> *unobstructedPoints) {
  std::shared_ptr<GameUnit> unit = gameState->GetUnit(target);
  if (!unit) return false;

  if (!IsInRange(*unit)) return false;

  std::vector<Vector2f> points;
  if (!unobstructedPoints) unobstructedPoints = &points;

  bool isBlocked = false;
  *unobstructedPoints = UnobstructedPointsOnTarget(target, AttackPoint());
  if (!unobstructedPoints->size()) {
    ChangeAttackPoint();
    *unobstructedPoints = UnobstructedPointsOnTarget(target, AttackPoint());
    if (!unobstructedPoints->size()) {
      ChangeAttackPoint();
      isBlocked = true;
    }
  }
  return !isBlocked;
}

void AttackAbility::Attack(std::shared_ptr<GameUnit> unit,
                           std::vector<Vector2f> unobstructdPoints) {
  owner->SetRotation(Util::FindAngle(unit->Position()
    - owner->Position()));

  if (loadTime <= 0.f) {

    float minDistance = FLT_MAX;
    Vector2f targetPoint;

    for (Vector2f point : unobstructdPoints) {
      float distance = Util::Distance(point, unit->Position());
      if (distance < minDistance) {
        minDistance = distance;
        targetPoint = point;
      }
    }

    float targetAngle = Util::FindAngle(targetPoint - AttackPoint());
    gameState->AddProjectile(owner, AttackPoint(),
      targetAngle, damage, 200.f);
    loadTime += 10.f;
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

std::vector<Vector2f> AttackAbility::UnobstructedPointsOnTarget(
    UnitId target, const Vector2f &attackPoint) const {

  auto unit = gameState->GetUnit(target);

  Circle circle(unit->Position(), unit->Attributes().CollisionRadius());
  auto widestPoints = circle.WidestPoints(attackPoint);

  std::vector<Vector2f> result;

  int nRayCasts = 9;

  for (int i = 0; i < nRayCasts; ++i) {
    float a = float(i) / nRayCasts;
    float b = 1 - a;
    Vector2f interpolatedPoint = a * widestPoints[0] + b * widestPoints[1];
    CollisionTestResult collision =
        gameState->TestCollision(attackPoint, interpolatedPoint, 0,
        owner);
    if (collision.unitHit == unit) {
      result.push_back(interpolatedPoint);
    }
  }
  return result;
}

std::vector<Vector2f> AttackAbility::UnobstructedPointsOnTarget(
    const Vector2f &attackPoint) const {
  return UnobstructedPointsOnTarget(target, attackPoint);
}