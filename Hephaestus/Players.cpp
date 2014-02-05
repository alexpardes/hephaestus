#include "stdafx.h"
#include "Players.h"
#include "Command.h"
#include "UnitCommand.h"
#include "UnitAction.h"

Player::Player(GameState &gameState) : gameState(gameState) {
	selected_units_ = std::list<UnitId>();
	turn_queue_ = std::deque<const CommandTurn *>();

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
	if (turn_queue_.size() > 0) {
		const CommandTurn* front = turn_queue_.front();
		turn_queue_.pop_front();
    return front;
	} else {
	  return new CommandTurn();
  }
}

void Player::ExecuteTurn() {
  const CommandTurn *turn = PopCommandTurn();
  CommandTurn::const_iterator it = turn->begin();
  while (it != turn->end()) {
    const Command *command = *it++;
    command->Execute(*this);
    delete command;
  }
  delete turn;
}

void Player::GiveOrder(const UnitCommand *command) {
  for (std::list<UnitId>::const_iterator it = selected_units_.begin();
      it != selected_units_.end(); ++it) {
    std::shared_ptr<GameUnit> unit = gameState.GetUnit(*it);
    if (unit) {
      unit->SetAction(command->MakeUnitAction(gameState));
    }
  }
}

void Player::DeselectDeadUnits() {
	std::vector<std::list<UnitId>::const_iterator> dead_units;
	for (std::list<UnitId>::const_iterator unit = selected_units_.begin();
			unit != selected_units_.end(); ++unit) {
		if (!gameState.GetUnit(*unit)) {
			dead_units.push_back(unit);
		}
	}
	for (int i = 0; i < dead_units.size(); ++i) {
		selected_units_.erase(dead_units[i]);
	}
}

void Player::ClearSelection() {
  selected_units_.clear();
}

void Player::AddToSelection(UnitId id) {
  selected_units_.push_back(id);
}