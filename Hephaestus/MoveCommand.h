#ifndef MOVE_COMMAND_
#define MOVE_COMMAND_

#include "UnitCommand.h"

class MoveCommand : public UnitCommand {
	public:
		MoveCommand() { }
		MoveCommand(const Vector2f &location) : location_(location) { }
    virtual UnitAction *MakeUnitAction() const;

	private:
		friend class boost::serialization::access;
		Vector2f location_;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & boost::serialization::base_object<UnitCommand>(*this);
			ar & location_;
		}
};

#endif