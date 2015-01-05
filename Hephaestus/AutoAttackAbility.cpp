#include "stdafx.h"
#include "AutoAttackAbility.h"
#include "GameUnit.h"
#include "AttackAbility.h"
#include "GameState.h"

AutoAttackAbility::AutoAttackAbility(GameUnit &owner, GameState &gameState) :
    owner(owner), gameState(gameState) {
  
  attackAbility = static_cast<AttackAbility*>(owner.GetAbility("Attack"));
}

void AutoAttackAbility::Execute() {
  attackAbility->EnableMovement(false);
  if (target) {
    if (!target->IsAlive() || !attackAbility->CanAttack(target->Id())) {
      target = nullptr;
    }
  }
  else {
    // Chooses the target whose bearing is closest to the unit's current direction.
    float smallestAngle = FLT_MAX;
    std::shared_ptr<GameUnit> closestUnit;
    for (auto unit : gameState.Units()) {
      if (unit->Owner() == owner.Owner())
        continue;

      if (unit->TurnsVisible() < 6)
        continue;

      if (!attackAbility->CanAttack(unit->Id()))
        continue;

      float bearing = Util::FindAngle(unit->Position() - owner.Position());
      float angle = Util::AngleBetween(bearing, owner.Rotation());
      if (angle < smallestAngle) {
        smallestAngle = angle;
        closestUnit = unit;
      }
    }

    target = closestUnit;
    if (target) {
      attackAbility->SetTarget(closestUnit->Id());
    }
  }
}