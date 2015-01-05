#include "stdafx.h"
#include "StopCommand.h"
#include "MoveAbility.h"

CommandType StopCommand::type = 0;

void StopCommand::SerializeInternal(char *&bytes) const { }

void StopCommand::Execute(GameState &gameState, GameUnit &unit) const {
  unit.SetAction(nullptr);
  static_cast<MoveAbility*>(unit.GetAbility("Move"))->Stop();
}

void StopCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::shared_ptr<Command> StopCommand::Deserialize(char *&bytes) {
  return std::make_shared<StopCommand>();
}

std::string StopCommand::ToString() const {
  return "Stop";
}