#pragma once
#include "UnitCommand.h"

class StopCommand : public UnitCommand {
  public:
    virtual UnitAction *MakeUnitAction(GameState &gameState) const;
    virtual std::string ToString() const;
    static void Register();

protected:
  virtual void SerializeInternal(char *&bytes) const;
  virtual CommandType Type() const { return type; }

  private:
    static CommandType type;
    static std::shared_ptr<Command> Deserialize(char *&bytes);
};