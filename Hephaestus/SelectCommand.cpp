#include "stdafx.h"
#include "SelectCommand.h"

void SelectCommand::Execute(Player &player) const {
  player.ClearSelection();
  for (size_t i = 0; i < targets_.size(); ++i) {
    player.AddToSelection(targets_[i]);
  }
}