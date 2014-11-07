#ifndef UNIT_COMMAND_
#define UNIT_COMMAND_

#include "Player.h"
#include "Command.h"
#include "UnitAction.h"
#include "GameState.h"

class UnitCommand : public Command {
	public:   
		virtual void Execute(Player &player) const;
    virtual UnitAction *MakeUnitAction(GameState &gameState) const = 0;
};

#endif