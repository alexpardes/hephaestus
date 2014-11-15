#include "stdafx.h"
#include "CommandTurn.h"
#include "Serialization.h"

void CommandTurn::Serialize(char *&bytes) const {
  Serialization::Write(bytes, hash);
  Serialization::Write(bytes, (char) commands.size());
  for (auto command : commands) {
    command->Serialize(bytes);
  }
}

std::shared_ptr<CommandTurn> CommandTurn::Deserialize(char *&bytes) {
  auto result = std::make_shared<CommandTurn>();
  result->hash = Serialization::Read<size_t>(bytes);
  auto nCommands = Serialization::Read<char>(bytes);
  for (char i = 0; i < nCommands; ++i) {
    result->commands.push_back(Command::Deserialize(bytes));
  }
  return result;
}