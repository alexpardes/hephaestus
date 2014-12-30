#include "stdafx.h"
#include "Player.h"
#include "StrafePointCommand.h"
#include "Serialization.h"
#include  "AutoAttackAbility.h"

CommandType StrafePointCommand::type = 0;

StrafePointCommand::StrafePointCommand(const Vector2f &point) {
  this->point = point;
}

void StrafePointCommand::Execute(GameState &gameState, GameUnit &unit) const {
  static_cast<AutoAttackAbility*>(unit.GetAbility("AutoAttack"))->SetTargetPoint(point);
}

void StrafePointCommand::SerializeInternal(char *&bytes) const {
  Serialization::Write(bytes, point);
}

std::shared_ptr<Command> StrafePointCommand::Deserialize(char *&bytes) {
  auto point = Serialization::Read<Vector2f>(bytes);
  return std::make_shared<StrafePointCommand>(point);
}

void StrafePointCommand::Register() {
  type = Command::Register(&Deserialize);
}

std::string StrafePointCommand::ToString() const {
  return "Strafe";
}

