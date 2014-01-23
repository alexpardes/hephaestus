#include "stdafx.h"
#include "MoveAction.h"

void MoveAction::Start(UnitAbility *ability) {
  this->ability = dynamic_cast<MoveAbility*>(ability);
  this->ability->SetDestination(location_);
}

void MoveAction::Execute() {
  ability->Execute();
}

std::string MoveAction::Name() {
  return "Move";
}