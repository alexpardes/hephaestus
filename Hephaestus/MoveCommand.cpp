#include "stdafx.h"
#include "MoveCommand.h"
#include "MoveAction.h"

UnitAction *MoveCommand::MakeUnitAction(GameState &gameState) const {
  return new MoveAction(location);
}