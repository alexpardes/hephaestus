#pragma once

#include "UnitCommand.h"

class GameState;

class AttackMoveCommand : public UnitCommand {
	public:
		AttackMoveCommand() { }
		AttackMoveCommand(const Vector2f &location): location_(location) { }
		virtual UnitAction *MakeUnitAction(GameState &gameState) const;

	private:
		friend class boost::serialization::access;
		Vector2f location_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
			ar & location_;
		}
};