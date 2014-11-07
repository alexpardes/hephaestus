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

std::vector<unsigned char> AttackCommand::Serialize() const {
  std::vector<unsigned char> bytes;
  Serialization::Add(bytes, target);
  return bytes;
}

std::shared_ptr<Command> AttackCommand::Deserialize(ByteIterator start, ByteIterator end) {
  return std::make_shared<AttackCommand>(Serialization::Deserialize<UnitId>(start));
}

void AttackCommand::Register() {
  type = Command::Register(&Deserialize);
}