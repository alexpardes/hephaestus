#include "stdafx.h"
#include "AttackCommand.h"
#include "AttackAction.h"

UnitAction* AttackCommand::MakeUnitAction(GameState &gameState) const {
  std::shared_ptr<GameUnit> unit = gameState.GetUnit(target);
  UnitAction* result = nullptr;
  if (unit) {
    result = new AttackAction(unit->Id());
  }
  return result;
}