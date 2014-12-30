#pragma once

#include "ActionCommand.h"

class GameState;

class AttackMoveCommand : public ActionCommand {
	public:
		AttackMoveCommand() { }
		AttackMoveCommand(const Vector2f &location): location(location) { }
		virtual UnitAction *MakeUnitAction(GameState &gameState) const;
    static std::shared_ptr<Command> Deserialize(char *&bytes);
    virtual std::string ToString() const;
    static void Register();

  protected:
    virtual void SerializeInternal(char *&bytes) const;
    virtual CommandType Type() const { return type; }

	private:
    static CommandType type;
		Vector2f location;
};