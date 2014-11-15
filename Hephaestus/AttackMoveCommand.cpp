#include "stdafx.h"
#include "AttackMoveCommand.h"
#include "AttackMoveAction.h"
#include "Serialization.h"

CommandType AttackMoveCommand::type = 0;

UnitAction *AttackMoveCommand::MakeUnitAction(GameState &gameState) const {
  return new AttackMoveAction(location);
}

void AttackMoveCommand::SerializeInternal(char *&bytes) const {
  Serialization::Write(bytes, location);
}

std::shared_ptr<Command> AttackMoveCommand::Deserialize(char *&bytes) {
  return std::make_shared<AttackMoveCommand>(Serialization::Read<Vector2f>(bytes));
}

void AttackMoveCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::string AttackMoveCommand::ToString() const {
  return "Attack Move";
}