#pragma once

#include "UnitCommand.h"

class GameState;

class AttackCommand : public UnitCommand {
	public:
		AttackCommand(UnitId target): target(target) { }
    virtual UnitAction *MakeUnitAction(GameState &gameState) const;
    virtual std::vector<unsigned char> Serialize() const;
    virtual CommandType Type() const { return type; }
    static std::shared_ptr<Command> Deserialize(ByteIterator start, ByteIterator end);
    static void Register();

	private:
    static CommandType type;
		UnitId target;
};