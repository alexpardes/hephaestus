#include "stdafx.h"
#include "Player.h"
#include "Command.h"
#include "UnitCommand.h"
#include "UnitAction.h"

Player::Player(GameState& gameState, CommandSource* commandSource) :
    gameState(gameState), commandSource(commandSource) { }

bool Player::QueueCommandTurn() {
  bool succeeded = false;
  auto turn = commandSource->TakeCommandTurn();
  if (turn) {
    turnQueue.push_back(turn);
    succeeded = true;
  }
  return succeeded;
}

const std::shared_ptr<CommandTurn> Player::PopCommandTurn() {
	if (turnQueue.size() > 0) {
		const std::shared_ptr<CommandTurn> front = turnQueue.front();
		turnQueue.pop_front();
    return front;
	} else {
	  return std::make_shared<CommandTurn>();
  }
}

void Player::ExecuteTurn() {
  const std::shared_ptr<CommandTurn> turn = PopCommandTurn();
  CommandTurn::const_iterator it = turn->begin();
  while (it != turn->end()) {
    const std::shared_ptr<const Command> command = *it++;
    command->Execute(*this);
  }
}

void Player::GiveOrder(const UnitCommand *command) {
  for (std::list<UnitId>::const_iterator it = selectedUnits.begin();
      it != selectedUnits.end(); ++it) {
    std::shared_ptr<GameUnit> unit = gameState.GetUnit(*it);
    if (unit) {
      unit->SetAction(command->MakeUnitAction(gameState));
    }
  }
}

void Player::SetCommandSource(CommandSource* source) {
  commandSource = source;
}

void Player::DeselectDeadUnits() {
	std::vector<std::list<UnitId>::const_iterator> dead_units;
	for (std::list<UnitId>::const_iterator unit = selectedUnits.begin();
			unit != selectedUnits.end(); ++unit) {
		if (!gameState.GetUnit(*unit)) {
			dead_units.push_back(unit);
		}
	}
  for (std::list<UnitId>::const_iterator deadUnit : dead_units) {
    selectedUnits.erase(deadUnit);
  }
}

void Player::ClearSelection() {
  selectedUnits.clear();
}

void Player::AddToSelection(UnitId id) {
  selectedUnits.push_back(id);
}