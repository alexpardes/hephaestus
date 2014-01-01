#include "stdafx.h"
#include "Players.h"

Player::Player() {
	selected_units_ = std::list<GameUnit *>();
	turn_queue_ = std::deque<const CommandTurn *>();
}

const std::list<GameUnit *> &Player::selected_units() const {
	return selected_units_;
}

Player &Players::GetPlayer(PlayerNumber number) {
	if (number == kPlayer1) {
		return player1_;
	} else {
		return player2_;
	}
}

void Player::AddCommandTurn(const CommandTurn *turn) {
	turn_queue_.push_back(turn);
}

const CommandTurn *Player::PopCommandTurn() {
	const CommandTurn *front = NULL;
	if (turn_queue_.size() > 0) {
		front = turn_queue_.front();
		turn_queue_.pop_front();
	}
	return front;
}

void Player::DeselectDeadUnits() {
	std::vector<std::list<GameUnit *>::const_iterator> dead_units;
	for (std::list<GameUnit *>::const_iterator unit = selected_units_.begin();
			unit != selected_units_.end(); ++unit) {
		if (!*unit) {
			dead_units.push_back(unit);
		}
	}
	for (int i = 0; i < dead_units.size(); ++i) {
		selected_units_.erase(dead_units[i]);
	}
}