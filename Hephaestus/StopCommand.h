#pragma once
#include "UnitCommand.h"

class StopCommand : public UnitCommand {
  public:
    virtual UnitAction *MakeUnitAction(GameState &gameState) const;
    virtual std::vector<unsigned char> Serialize() const;
    virtual CommandType Type() const { return type; }
    static void Register();

  private:
    static CommandType type;
    static std::shared_ptr<Command> Deserialize(ByteIterator start, ByteIterator end);
};