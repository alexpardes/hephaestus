#include "stdafx.h"
#include "AttackMoveCommand.h"
#include "AttackMoveAction.h"
#include "Serialization.h"

CommandType AttackMoveCommand::type = 0;

UnitAction *AttackMoveCommand::MakeUnitAction(GameState &gameState) const {
  return new AttackMoveAction(location);
}

std::vector<unsigned char> AttackMoveCommand::Serialize() const {
  std::vector<unsigned char> bytes;
  Serialization::Add(bytes, location);
  return bytes;
}

std::shared_ptr<Command> AttackMoveCommand::Deserialize(ByteIterator start, ByteIterator end) {
  return std::make_shared<AttackMoveCommand>(Serialization::Deserialize<Vector2f>(start));
}

void AttackMoveCommand::Register() {
  type = Command::Register(&Deserialize);
}