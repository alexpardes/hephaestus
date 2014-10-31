#include "stdafx.h"
#include "CommandSource.h"

CommandSource* CommandSource::Null = new CommandSource::NullCommandSource();

std::shared_ptr<CommandTurn> CommandSource::NullCommandSource::TakeCommandTurn() {
  return std::make_shared<CommandTurn>();
}