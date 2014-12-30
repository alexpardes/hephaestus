#pragma once
#include "Player.h"
#include "UnitCommand.h"
#include "UnitAction.h"
#include "GameState.h"

class ActionCommand : public UnitCommand {
  public:
    virtual void Execute(GameState &gameState, GameUnit &unit) const;

  protected:
    virtual UnitAction *MakeUnitAction(GameState &gameState) const = 0;
};