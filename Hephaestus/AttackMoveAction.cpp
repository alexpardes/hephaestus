#include "stdafx.h"
#include "AttackMoveAction.h"
#include "AttackMoveAbility.h"
#include "GameUnit.h"

void AttackMoveAction::Start(GameUnit &unit) {
  ability = static_cast<AttackMoveAbility*>(unit.GetAbility("AttackMove"));
  ability->SetDestination(destination);
}

void AttackMoveAction::Execute(GameUnit &unit) {
  ability->Execute();
}

bool AttackMoveAction::IsFinished() const {
  return ability->DestinationReached();
}