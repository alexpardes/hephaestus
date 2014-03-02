#include "stdafx.h"
#include "AttackAbility.h"
#include "GameState.h"
#include "Util.h"
#include "MoveAbility.h"
#include <Scripting/LuaUtilities.h>

AttackAbility::AttackAbility(std::shared_ptr<GameUnit> owner,
                             GameState *gameState,
                             float damage,
                             float speed,
                             float range) :
    owner(owner), gameState(gameState), damage(damage), speed(speed),
    range(range), loadTime(0) {

  moveAbility = dynamic_cast<MoveAbility*>(owner->GetAbility("Move"));
}

void AttackAbility::Execute() {

  std::shared_ptr<GameUnit> unit = gameState->GetUnit(target);
  if (!unit) return;

  if (IsInRange() && IsUnobstructed()) {
    owner->SetRotation(Util::FindAngle(unit->Position()
      - owner->Position()));

    if (loadTime <= 0.f) {
      gameState->AddProjectile(owner->Attributes().name(), owner->Position(),
        unit, damage, 125.f);
      loadTime += 10.f;
    }
  } else {
    moveAbility->SetDestination(unit->Position());
    moveAbility->Execute();
  }
  loadTime -= speed;
}

bool AttackAbility::IsInRange() const {
  bool result = false;
  std::shared_ptr<GameUnit> unit = gameState->GetUnit(target);
  if (unit) {
    if (Util::Distance(unit->Position(), owner->Position()) < range) {
      result = true;
    }
  }
  return result;
}

bool AttackAbility::IsUnobstructed() const {
  std::shared_ptr<GameUnit> unit = gameState->GetUnit(target);
  float distanceToTarget = Util::Distance(owner->Position(), unit->Position())
      - unit->Attributes().CollisionRadius();
  float distanceToObstacle = gameState->DistanceToObstacle(*owner,
      unit->Position(), 0.f);
  float tolerance = 0.01f;
  return distanceToObstacle >= distanceToTarget - tolerance;
}