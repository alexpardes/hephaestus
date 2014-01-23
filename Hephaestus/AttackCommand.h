#ifndef ATTACK_COMMAND_
#define ATTACK_COMMAND_

#include "Command.h"

class AttackCommand : public Command {
	public:
		AttackCommand() { }
		AttackCommand(UnitId target): target_(target) { }
		~AttackCommand() { }
		virtual void Execute(Player &player) { }

	private:
		friend class boost::serialization::access;
		UnitId target_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & target_;
		}
};

#endif
