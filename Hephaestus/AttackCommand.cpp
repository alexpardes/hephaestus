#include "stdafx.h"
#include "AttackCommand.h"
#include "AttackAction.h"

UnitAction *AttackCommand::MakeUnitAction(GameState &gameState) const {
  GameUnit &unit = *gameState.GetUnit(target);
  return new AttackAction(unit);
}