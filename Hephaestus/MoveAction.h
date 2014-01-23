#ifndef MOVE_ACTION_
#define MOVE_ACTION_

#include "Util.h"
#include "UnitAction.h"
#include "MoveAbility.h"

class MoveAction : public UnitAction {
	public:
		MoveAction(const Vector2f &location) {
      location_ = location;
    }

    virtual void Start(UnitAbility *ability);
    virtual void Execute();
    virtual std::string Name();

	private:
		Vector2f location_;
    MoveAbility *ability;
};

#endif