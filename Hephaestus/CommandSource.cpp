#include "stdafx.h"
#include "CommandSource.h"

CommandSource* CommandSource::Null = new CommandSource::NullCommandSource();

CommandTurn* CommandSource::NullCommandSource::TakeCommandTurn() {
  return new CommandTurn();
}