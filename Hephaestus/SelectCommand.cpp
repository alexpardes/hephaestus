#include "stdafx.h"
#include "SelectCommand.h"
#include "Player.h"
#include "Serialization.h"

CommandType SelectCommand::type = 0;

SelectCommand::SelectCommand(const SelectCommand &other) {
  targets = std::vector<UnitId>(other.targets);
}

void SelectCommand::Execute(Player &player) const {
  player.ClearSelection();
  for (UnitId target : targets) {
    player.AddToSelection(target);
  }
}

std::vector<unsigned char> SelectCommand::Serialize() const {
  std::vector<unsigned char> bytes;
  for (auto target : targets) {
    Serialization::Add(bytes, target);
  }
  return bytes;
}

std::shared_ptr<Command> SelectCommand::Deserialize(ByteIterator start, ByteIterator end) {
  std::vector<UnitId> targets;
  while (start != end) {
    targets.push_back(Serialization::Deserialize<UnitId>(start));
  }
  return std::make_shared<SelectCommand>(targets);
}

void SelectCommand::Register() {
  type = Command::Register(&Deserialize);
}