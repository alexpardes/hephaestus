#include "stdafx.h"
#include "MoveCommand.h"
#include "MoveAction.h"
#include "Serialization.h"

CommandType MoveCommand::type = 0;

UnitAction *MoveCommand::MakeUnitAction(GameState &gameState) const {
  return new MoveAction(location);
}

void MoveCommand::SerializeInternal(char *&bytes) const {
  Serialization::Write(bytes, location);
}

std::shared_ptr<Command> MoveCommand::Deserialize(char *&bytes) {
  return std::make_shared<MoveCommand>(Serialization::Read<Vector2f>(bytes));
}

void MoveCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::string MoveCommand::ToString() const {
  return "Move";
}