#pragma once
#include "Command.h"

class UnitCommand : public Command {
  public:
    virtual void Execute(Player &player) const;
    virtual void Execute(GameState &gameState, GameUnit &unit) const = 0;
};