#pragma once

#include "UnitCommand.h"

class MoveCommand : public UnitCommand {
	public:
		MoveCommand() { }
		MoveCommand(const Vector2f &location) : location(location) { }
    virtual UnitAction *MakeUnitAction(GameState &gameState) const;

	private:
		friend class boost::serialization::access;
		Vector2f location;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<UnitCommand>(*this);
			ar & location;
		}
};