#include "stdafx.h"
#include "AttackMoveCommand.h"
#include "AttackMoveAction.h"

UnitAction *AttackMoveCommand::MakeUnitAction(GameState &gameState) const {
  return new AttackMoveAction(location_);
}
