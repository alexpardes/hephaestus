#include "stdafx.h"
#include "SelectCommand.h"

void SelectCommand::Execute(Player &player) const {
  player.ClearSelection();
  for (UnitId target : targets_) {
    player.AddToSelection(target);
  }
}