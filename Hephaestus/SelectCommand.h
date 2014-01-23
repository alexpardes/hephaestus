#pragma once

#include "Command.h"

class SelectCommand : public Command {
	public:
		SelectCommand() { } // Default constructor for serialization. Probably can be made private. 12/25/2013
		SelectCommand(const std::vector<UnitId> &targets) : targets_(targets) { }
		~SelectCommand() { }
		virtual void Execute(Player &player) const;

	private:
		friend class boost::serialization::access;
		std::vector<UnitId> targets_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & targets_;
		}
};