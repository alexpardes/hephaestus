#pragma once

#include "GameUnit.h"
#include "GameState.h"
#include "CommandSource.h"
#include <deque>

class Command;
class UnitCommand;
class CommandTurn;

class Player {
	public:
    Player(GameState& gameState,
        CommandSource* commandSource = CommandSource::Null);
    bool QueueCommandTurn();
    void ExecuteTurn();
    void ClearSelection();
    void AddToSelection(UnitId id);
    void GiveOrder(const UnitCommand *command);
    void SetCommandSource(CommandSource* source);
    const CommandTurn &NextTurn() const;

	private:
    void DeselectDeadUnits();
    const std::shared_ptr<CommandTurn> PopCommandTurn();
		std::list<UnitId> selectedUnits;
		std::deque<const std::shared_ptr<CommandTurn>> turnQueue;
    GameState& gameState;
    CommandSource* commandSource;
};