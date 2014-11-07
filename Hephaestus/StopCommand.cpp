#include "stdafx.h"
#include "StopCommand.h"

CommandType StopCommand::type = 0;

std::vector<unsigned char> StopCommand::Serialize() const {
  return std::vector<unsigned char>();
}

UnitAction *StopCommand::MakeUnitAction(GameState &gameState) const {
  return nullptr;
}

void StopCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::shared_ptr<Command> StopCommand::Deserialize(ByteIterator start, ByteIterator end) {
  return std::make_shared<StopCommand>();
}