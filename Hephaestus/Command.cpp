#include "stdafx.h"
#include "Command.h"

std::vector<Command::Deserializer> Command::deserializers;

CommandType Command::Register(Deserializer deserializer) {
  deserializers.push_back(deserializer);
  return (CommandType) deserializers.size();
}

std::shared_ptr<Command> Command::Deserialize(CommandType type, ByteIterator start, ByteIterator end) {
  return deserializers[type - 1](start, end);
}