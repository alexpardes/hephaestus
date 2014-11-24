#include "stdafx.h"
#include "AttackAction.h"
#include "AttackAbility.h"
#include "MoveAbility.h"

void AttackAction::Start(GameUnit &unit) {
  UnitAbility *ability = unit.GetAbility("Attack");
  if (ability) {
    attackAbility = static_cast<AttackAbility*>(ability);
    attackAbility->SetTarget(target);
  }
  ability = unit.GetAbility("Move");
  if (ability) {
    moveAbility = static_cast<MoveAbility*>(ability);
  }
}

void AttackAction::Execute(GameUnit &unit) {
  attackAbility->Execute();
}

bool AttackAction::IsFinished() const {
  return attackAbility->Target() == nullptr;
}

std::string AttackAction::Name() const {
  return "Attack";
}