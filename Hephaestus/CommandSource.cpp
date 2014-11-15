#include "stdafx.h"
#include "CommandSource.h"
#include "CommandTurn.h"

CommandSource* CommandSource::Null = new CommandSource::NullCommandSource();

std::shared_ptr<CommandTurn> CommandSource::NullCommandSource::TakeCommandTurn() {
  return std::make_shared<CommandTurn>(gameHash);
}

void CommandSource::NullCommandSource::SetGameHash(size_t gameHash) {
  this->gameHash = gameHash;
}