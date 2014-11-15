#include "stdafx.h"
#include "Command.h"
#include "Serialization.h"

std::vector<Command::Deserializer> Command::deserializers;

CommandType Command::Register(Deserializer deserializer) {
  deserializers.push_back(deserializer);
  return (CommandType) deserializers.size();
}

void Command::Serialize(char *&bytes) const {
  Serialization::Write(bytes, Type());
  SerializeInternal(bytes);
}

std::shared_ptr<Command> Command::Deserialize(char *&bytes) {
  auto type = Serialization::Read<char>(bytes);
  return deserializers[type - 1](bytes);
}