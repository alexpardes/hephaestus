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

void SelectCommand::SerializeInternal(char *&bytes) const {
  Serialization::Write(bytes, (char) targets.size());
  for (auto target : targets) {
    Serialization::Write(bytes, target);
  }
}

std::shared_ptr<Command> SelectCommand::Deserialize(char *&bytes) {
  auto nTargets = Serialization::Read<char>(bytes);
  std::vector<UnitId> targets;
  for (char i = 0; i < nTargets; ++ i) {
    targets.push_back(Serialization::Read<UnitId>(bytes));
  }
  return std::make_shared<SelectCommand>(targets);
}

void SelectCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::string SelectCommand::ToString() const {
  return "Select";
}