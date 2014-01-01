#ifndef PLAYERS_
#define PLAYERS_

#include "Commands.h"
#include "GameUnit.h"

class Player {
	public:
		Player();

		const std::list<GameUnit *> &selected_units() const;

		void set_selected_units(const std::list<GameUnit *> &units) {
			selected_units_ = units;
		}

		void AddCommandTurn(const CommandTurn *turn);
		const CommandTurn *PopCommandTurn();
		void DeselectDeadUnits();

	private:
		std::list<GameUnit *> selected_units_;
		std::deque<const CommandTurn *> turn_queue_;
};

class Players {
	public:
		Players() : player1_(), player2_() { }
		Player &GetPlayer(PlayerNumber number);

	private:
		Player player1_, player2_;
};

#endif