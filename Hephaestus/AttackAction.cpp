#include "stdafx.h"
#include "AttackAction.h"
#include "AttackAbility.h"
#include "MoveAbility.h"

void AttackAction::Start(GameUnit &unit) {
  UnitAbility *ability = unit.GetAbility("Attack");
  if (ability) {
    attackAbility = dynamic_cast<AttackAbility*>(ability);
    attackAbility->SetTarget(target);
  }
  ability = unit.GetAbility("Move");
  if (ability) {
    moveAbility = dynamic_cast<MoveAbility*>(ability);
  }
}

void AttackAction::Execute(GameUnit &unit) {
  attackAbility->Execute();
}

std::string AttackAction::Name() {
  return "Attack";
}