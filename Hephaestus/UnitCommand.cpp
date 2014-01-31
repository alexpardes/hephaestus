#include "stdafx.h"
#include "UnitCommand.h"

void UnitCommand::Execute(Player &player) const {
  player.GiveOrder(this);
}