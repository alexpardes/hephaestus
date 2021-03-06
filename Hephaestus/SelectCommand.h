#pragma once

#include "Command.h"

class SelectCommand : public Command {
	public:
		SelectCommand(const std::vector<UnitId> &targets) : targets(targets) { }
    SelectCommand(const SelectCommand &other);
    ~SelectCommand() { }
    virtual void Execute(Player &player) const;
    virtual std::string ToString() const;
    static std::shared_ptr<Command> Deserialize(char *&bytes);
    static void Register();

protected:
  virtual void SerializeInternal(char *&bytes) const;
  virtual CommandType Type() const { return type; }

	private:
    static CommandType type;
		std::vector<UnitId> targets;
};