#ifndef SELECT_COMMAND_
#define SELECT_COMMAND_

#include "Command.h"

class SelectCommand : public Command {
	public:
		SelectCommand() { } // Default constructor for serialization. Probably can be made private. 12/25/2013
		SelectCommand(const std::list<unitId> &targets) :
				/*Command(kSelect),*/ targets_(targets) { }
		~SelectCommand() { }
		CommandType type() const { return Command::kSelect; }
		const std::list<unitId> &targets() const {return targets_;}
		virtual void Execute(const std::vector<GameUnit> *units) { }

	private:
		friend class boost::serialization::access;
		std::list<unitId> targets_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & targets_;
		}
};

#endif