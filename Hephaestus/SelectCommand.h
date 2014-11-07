#pragma once

#include "Command.h"

class SelectCommand : public Command {
	public:
		SelectCommand(const std::vector<UnitId> &targets) : targets(targets) { }
    SelectCommand(const SelectCommand &other);
    ~SelectCommand() { }
    virtual void Execute(Player &player) const;
    virtual std::vector<unsigned char> Serialize() const;
    virtual CommandType Type() const { return type; }
    static std::shared_ptr<Command> Deserialize(ByteIterator start, ByteIterator end);
    static void Register();

	private:
    static CommandType type;
		std::vector<UnitId> targets;
};