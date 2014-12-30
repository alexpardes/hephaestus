#pragma once
#include "UnitCommand.h"

class StrafePointCommand : public UnitCommand {
  public:
    StrafePointCommand(const Vector2f &point);
    virtual void Execute(GameState &gameState, GameUnit &unit) const;
    virtual std::string ToString() const;
    static std::shared_ptr<Command> Deserialize(char *&bytes);
    static void Register();

  protected:
    virtual void SerializeInternal(char *&bytes) const;
    virtual CommandType Type() const { return type; }

private:
  static CommandType type;
  Vector2f point;
};