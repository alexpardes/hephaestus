#include "stdafx.h"
#include "StopCommand.h"

CommandType StopCommand::type = 0;

void StopCommand::SerializeInternal(char *&bytes) const { }

UnitAction *StopCommand::MakeUnitAction(GameState &gameState) const {
  return nullptr;
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