#include "stdafx.h"
#include "TargetGroundAction.h"
#include "TargetGroundAbility.h"
#include "GameUnit.h"

TargetGroundAction::TargetGroundAction(const Vector2f &location, bool fullCover) {
  this->location = location;
  this->fullCover = fullCover;
}

void TargetGroundAction::Start(GameUnit &unit) {
  ability = static_cast<TargetGroundAbility*>(unit.GetAbility("TargetGround"));
  ability->SetDestination(location, fullCover);
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