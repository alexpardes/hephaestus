#include "stdafx.h"
#include "MoveAction.h"
#include "MoveAbility.h"
#include "GameUnit.h"

void MoveAction::Start(GameUnit &unit) {
  UnitAbility *ability = unit.GetAbility("Move");
  this->ability = dynamic_cast<MoveAbility*>(ability);
  this->ability->SetDestination(location);
}

void MoveAction::Execute(GameUnit &unit) {
  ability->Execute();
}

std::string MoveAction::Name() const {
  return "Move";
}

bool MoveAction::IsFinished() const {
  return ability->DestinationReached();
}