#include "stdafx.h"
#include "TargetGroundAction.h"
#include "TargetGroundAbility.h"
#include "GameUnit.h"

TargetGroundAction::TargetGroundAction(const Vector2f &location) {
  this->location = location;
}

void TargetGroundAction::Start(GameUnit &unit) {
  ability = static_cast<TargetGroundAbility*>(unit.GetAbility("TargetGround"));
  ability->SetDestination(location);
}

void TargetGroundAction::Execute(GameUnit &unit) {
  ability->Execute();
}

std::string TargetGroundAction::Name() const {
  return "TargetGround";
}

bool TargetGroundAction::IsFinished() const {
  return ability->DestinationReached();
}