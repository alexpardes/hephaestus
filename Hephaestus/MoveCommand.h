#ifndef MOVE_COMMAND_
#define MOVE_COMMAND_

#include "Command.h"

class MoveCommand : public Command {
	public:
		MoveCommand() { }
		MoveCommand(const Vector2i &location) : location_(location) { }
		~MoveCommand() { }
		CommandType type() const { return Command::kMove; }
		const Vector2i &location() const {return location_;}
		virtual void Execute(const std::vector<GameUnit> *units) { }

	private:
		friend class boost::serialization::access;
		Vector2i location_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & location_;
		}
};

#endif