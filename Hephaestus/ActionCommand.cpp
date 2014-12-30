#include "stdafx.h"
#include "ActionCommand.h"

void ActionCommand::Execute(GameState &gameState, GameUnit &unit) const {
  unit.SetAction(MakeUnitAction(gameState));
}