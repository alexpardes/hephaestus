#include "stdafx.h"
#include "TargetGroundCommand.h"
#include "TargetGroundAction.h"
#include "Serialization.h"

CommandType TargetGroundCommand::type = 0;

UnitAction *TargetGroundCommand::MakeUnitAction(GameState &gameState) const {
  return new TargetGroundAction(location);
}

void TargetGroundCommand::SerializeInternal(char *&bytes) const {
  Serialization::Write(bytes, location);
}

std::shared_ptr<Command> TargetGroundCommand::Deserialize(char *&bytes) {
  return std::make_shared<TargetGroundCommand>(Serialization::Read<Vector2f>(bytes));
}

void TargetGroundCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::string TargetGroundCommand::ToString() const {
  return "TargetGround";
}