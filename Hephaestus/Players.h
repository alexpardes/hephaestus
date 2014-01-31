#pragma once

#include "GameUnit.h"
#include "GameState.h"
#include <deque>

class Command;
class UnitCommand;
typedef std::vector<const Command*> CommandTurn;

class Player {
	public:
		Player(GameState &gameState);

		void AddCommandTurn(const CommandTurn *turn);
    void ExecuteTurn();
    void ClearSelection();
    void AddToSelection(UnitId id);
    void GiveOrder(const UnitCommand *command);

	private:
    void DeselectDeadUnits();
    const CommandTurn* PopCommandTurn();
		std::list<UnitId> selected_units_;
		std::deque<const CommandTurn *> turn_queue_;
    GameState &gameState;
};

class Players {
	public:
		Players(GameState &gameState) : player1_(gameState),
        player2_(gameState) { }
		Player &GetPlayer(PlayerNumber number);

	private:
		Player player1_, player2_;
};