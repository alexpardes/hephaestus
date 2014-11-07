#pragma once

#include "UnitCommand.h"

class GameState;

class AttackMoveCommand : public UnitCommand {
	public:
		AttackMoveCommand() { }
		AttackMoveCommand(const Vector2f &location): location(location) { }
		virtual UnitAction *MakeUnitAction(GameState &gameState) const;
    virtual std::vector<unsigned char> Serialize() const;
    virtual CommandType Type() const { return type; }
    static std::shared_ptr<Command> Deserialize(ByteIterator start, ByteIterator end);
    static void Register();

	private:
    static CommandType type;
		Vector2f location;
};