#ifndef PLAYERS_
#define PLAYERS_

#include "GameUnit.h"
#include "GameState.h"

class Command;
class UnitAction;
typedef std::vector<const Command*> CommandTurn;

class Player {
	public:
		Player(GameState &gameState);

		void AddCommandTurn(const CommandTurn *turn);
    void ExecuteTurn();
    void ClearSelection();
    void AddToSelection(UnitId id);
    void GiveOrder(UnitAction *action);

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

#endif