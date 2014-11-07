#include "stdafx.h"
#include "MoveCommand.h"
#include "MoveAction.h"
#include "Serialization.h"

CommandType MoveCommand::type = 0;

UnitAction *MoveCommand::MakeUnitAction(GameState &gameState) const {
  return new MoveAction(location);
}

std::vector<unsigned char> MoveCommand::Serialize() const {
  std::vector<unsigned char> bytes;
  Serialization::Add(bytes, location);
  return bytes;
}

std::shared_ptr<Command> MoveCommand::Deserialize(ByteIterator start, ByteIterator end) {
  return std::make_shared<MoveCommand>(Serialization::Deserialize<Vector2f>(start));
}

void MoveCommand::Register() {
  type = Command::Register(&Deserialize);
}