#include "stdafx.h"
#include "AttackCommand.h"
#include "AttackAction.h"
#include "Serialization.h"

CommandType AttackCommand::type = 0;

UnitAction* AttackCommand::MakeUnitAction(GameState &gameState) const {
  std::shared_ptr<GameUnit> unit = gameState.GetUnit(target);
  UnitAction* result = nullptr;
  if (unit) {
    result = new AttackAction(unit->Id());
  }
  return result;
}

void AttackCommand::SerializeInternal(char *&bytes) const {
  Serialization::Write(bytes, target);
}

std::shared_ptr<Command> AttackCommand::Deserialize(char *&bytes) {
  return std::make_shared<AttackCommand>(Serialization::Read<UnitId>(bytes));
}

void AttackCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::string AttackCommand::ToString() const {
  return "Attack";
}