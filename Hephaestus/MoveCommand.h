#pragma once

#include "ActionCommand.h"

class MoveCommand : public ActionCommand {
	public:
		MoveCommand() { }
		MoveCommand(const Vector2f &location) : location(location) { }
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