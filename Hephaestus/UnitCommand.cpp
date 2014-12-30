#include "stdafx.h"
#include "UnitCommand.h"
#include "Player.h"

void UnitCommand::Execute(Player &player) const {
  player.GiveOrder(*this);
}