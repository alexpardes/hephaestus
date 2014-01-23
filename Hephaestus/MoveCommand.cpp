#include "stdafx.h"
#include "MoveCommand.h"
#include "MoveAction.h"

UnitAction *MoveCommand::MakeUnitAction() const {
  return new MoveAction(location_);
}