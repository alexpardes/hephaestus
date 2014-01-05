#ifndef ATTACK_COMMAND_
#define ATTACK_COMMAND_

#include "Command.h"

class AttackCommand : public Command {
	public:
		AttackCommand() { }
		AttackCommand(unitId target): target_(target) { }
		~AttackCommand() { }
		Command::CommandType type() const { return Command::kAttack; }
		unitId target() const {return target_;}
		virtual void Execute(const std::vector<GameUnit> *units) { }

	private:
		friend class boost::serialization::access;
		unitId target_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & target_;
		}
};

#endif
