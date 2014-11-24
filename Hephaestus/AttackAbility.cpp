#include "stdafx.h"
#include "AttackAbility.h"
#include "GameState.h"
#include "Util.h"
#include "MoveAbility.h"
#include "Circle.h"

AttackAbility::AttackAbility(std::shared_ptr<GameUnit> owner,
                             GameState *gameState,
                             float damage,
                             float speed,
                             float range) :
    owner(owner), gameState(gameState), damage(damage), speed(speed),
    range(range), loadTime(0) {

  moveAbility = static_cast<MoveAbility*>(owner->GetAbility("Move"));
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

void AttackAbility::ChooseAttackPoint() {
  // Raycast forward from each attack point and use one with the longest line of sight.
  Vector2f sightVector = 1000.f * Util::MakeUnitVector(owner->Rotation());
  auto leftCollision = gameState->TestWallCollision(LeftAttackPoint(), LeftAttackPoint() + sightVector, 0);
  auto rightCollision = gameState->TestWallCollision(RightAttackPoint(), RightAttackPoint() + sightVector, 0);
  bool useLeftPoint = Util::Distance(leftCollision.point, LeftAttackPoint()) > Util::Distance(rightCollision.point, RightAttackPoint());
  owner->SetFacing(useLeftPoint ? kLeft : kRight);
}

Vector2f AttackAbility::AttackPoint() const {
  if (owner->Facing() == kLeft) {
    return LeftAttackPoint();
  } else {
    return RightAttackPoint();
  }
}

Vector2f AttackAbility::OtherAttackPoint() const {
  if (owner->Facing() != kLeft) {
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

void AttackAbility::Execute() {
  auto unit = gameState->GetUnit(target);
  if (unit) {
    if (CanAttack(target))
      Attack(*unit);
  }
  loadTime -= speed;
}

bool AttackAbility::CanAttack(UnitId target) const {
  auto unit = gameState->GetUnit(target);
  if (!unit || !IsInRange(*unit))
    return false;

  return owner->SightMap().IsAnyVisible(unit->SegmentFromUnit(AttackPoint()));
}

void AttackAbility::Attack(const GameUnit &unit) {
  ChooseAttackPoint();

  auto targetSegment = owner->SightMap().LargestVisibleSubsegment(unit.SegmentFromUnit(owner->Position()));
  auto targetPoint = (targetSegment.p1 + targetSegment.p2) / 2.f;
  float targetAngle = Util::FindAngle(targetPoint - AttackPoint());
  owner->SetRotation(Util::FindAngle(targetPoint - owner->Position()));

  if (loadTime <= 0.f) {
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

const std::shared_ptr<GameUnit> AttackAbility::Target() const {
  return gameState->GetUnit(target);
}