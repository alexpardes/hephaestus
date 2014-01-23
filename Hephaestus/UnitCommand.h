#ifndef UNIT_COMMAND_
#define UNIT_COMMAND_

#include "Players.h"
#include "Command.h"
#include "UnitAction.h"
#include "GameState.h"

class UnitCommand : public Command {
	public:   
		virtual void Execute(Player &player) const;

  protected:
    virtual UnitAction *MakeUnitAction() const = 0;

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version) {
			ar & boost::serialization::base_object<Command>(*this);
		}
};

#endif