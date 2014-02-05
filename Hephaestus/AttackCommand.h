#pragma once

#include "UnitCommand.h"

class GameState;

class AttackCommand : public UnitCommand {
	public:
		AttackCommand() { }
		AttackCommand(UnitId target): target(target) { }
		virtual UnitAction *MakeUnitAction(GameState &gameState) const;

	private:
		friend class boost::serialization::access;
		UnitId target;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & target;
		}
};