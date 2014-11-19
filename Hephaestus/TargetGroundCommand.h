#pragma once
#include "UnitCommand.h"

class TargetGroundCommand : public UnitCommand {
public:
  TargetGroundCommand(const Vector2f &location) : location(location) { }
  virtual UnitAction *MakeUnitAction(GameState &gameState) const;
  virtual std::string ToString() const;
  static std::shared_ptr<Command> Deserialize(char *&bytes);
  static void Register();

protected:
  virtual void SerializeInternal(char *&bytes) const;
  virtual CommandType Type() const { return type; }

private:
  static CommandType type;
  Vector2f location;
};