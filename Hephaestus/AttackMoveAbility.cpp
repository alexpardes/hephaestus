#include "stdafx.h"
#include "AttackMoveAbility.h"
#include "GameUnit.h"
#include "AttackAbility.h"
#include "MoveAbility.h"
#include "Util.h"
#include "GameState.h"

AttackMoveAbility::AttackMoveAbility(GameUnit &owner,
                                     GameState &gameState,
                                     float aggroRange,
                                     float deaggroRange) :
    owner(owner), gameState(gameState), aggroRange(aggroRange),
    deaggroRange(deaggroRange) {

  attackAbility = dynamic_cast<AttackAbility*>(owner.GetAbility("Attack"));
  moveAbility = dynamic_cast<MoveAbility*>(owner.GetAbility("Move"));
}

void AttackMoveAbility::SetDestination(const Vector2f &destination) {
  this->destination = destination;
  moveAbility->SetDestination(destination);
}

void AttackMoveAbility::Execute() {
  if (target) {
    if (target->IsAlive() && Util::Distance(target->Position(),
        owner.Position()) < deaggroRange) {
      attackAbility->Execute();
    } else {
      target = nullptr;
      moveAbility->SetDestination(destination);
    }
  } else {
    std::vector<std::shared_ptr<GameUnit>> nearbyUnits =
        gameState.GetUnitsInCircle(owner.Position(), aggroRange);
    float minimumDistance = FLT_MAX;
    std::shared_ptr<GameUnit> closestUnit;
    for (std::shared_ptr<GameUnit> unit : nearbyUnits) {
      if (unit->Owner() != owner.Owner()) {
        float distance = Util::Distance(unit->Position(), owner.Position());
        if (distance < minimumDistance) {
          minimumDistance = distance;
          closestUnit = unit;
        }
      }
    }

    if (minimumDistance <= aggroRange && closestUnit) {
      target = closestUnit;
      attackAbility->SetTarget(closestUnit->Id());
      attackAbility->Execute();
    } else {
      moveAbility->Execute();
    }
  }
}

