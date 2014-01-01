#ifndef COMMANDS_
#define COMMANDS_

#include "GameUnit.h"
#include "Util.h"
#include "VectorSerialization.h"

class Command {
	public:
		enum CommandType {kSelect, kMove, kAttack, kAttackMove};
		static CommandType IntToType(int i) {return CommandType(i);}
		const CommandType type() const {return type_;}
		Command() { }
		virtual ~Command() { }

	protected:
		Command(CommandType type) : type_(type) { }

	private:
		friend class boost::serialization::access;
		CommandType type_;	

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & type_;
		}	
};
typedef std::vector<const Command*> CommandTurn;

class SelectCommand : public Command {
	public:
		SelectCommand() { } // Default constructor for serialization. Probably can be made private. 12/25/2013
		SelectCommand(const std::list<unitId> &targets) :
				Command(kSelect), targets_(targets) { }
		~SelectCommand() { }
		const std::list<unitId> &targets() const {return targets_;}

	private:
		friend class boost::serialization::access;
		std::list<unitId> targets_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & targets_;
		}
};

class MoveCommand : public Command {
	public:
		MoveCommand() { }
		MoveCommand(const Vector2i &location) :
				Command(kMove), location_(location) { }
		~MoveCommand() { }
		const Vector2i &location() const {return location_;}

	private:
		friend class boost::serialization::access;
		Vector2i location_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & location_;
		}
};

class AttackCommand : public Command {
	public:
		AttackCommand() { }
		AttackCommand(unitId target) :
				Command(kAttack), target_(target) { }
		~AttackCommand() { }
		unitId target() const {return target_;}

	private:
		friend class boost::serialization::access;
		unitId target_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & target_;
		}
};

class AttackMoveCommand : public Command {
	public:
		AttackMoveCommand() { }
		AttackMoveCommand(const Vector2i &location):
			Command(kAttackMove), location_(location) { }
		~AttackMoveCommand() { }
		const Vector2i &location() const {return location_;}

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