#include "stdafx.h"
#include "Player.h"
#include "CommandTurn.h"
#include "UnitCommand.h"

Player::Player(GameState& gameState, CommandSource* commandSource) :
    gameState(gameState), commandSource(commandSource) { }

bool Player::QueueCommandTurn() {
  auto turn = commandSource->TakeCommandTurn();
  if (!turn)
    return false;
  
  turnQueue.push_back(turn);
  return true;
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
  for (auto command : *turn) {
    command->Execute(*this);
  }
}

void Player::GiveOrder(const UnitCommand &command) {
  for (auto unitId : selectedUnits) {
    auto unit = gameState.GetUnit(unitId);
    if (unit)
      command.Execute(gameState, *unit);
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

const CommandTurn &Player::NextTurn() const {
  return *turnQueue.front();
}