#ifndef UNIT_ACTION_
#define UNIT_ACTION_

class UnitAbility;
class GameUnit;

// An action represents a use of abilities. For example a MoveAbility
// contains the logic for moving the unit. A MoveAction passes the
// desired destination to the MoveAbility.
class UnitAction {
	public:
		virtual ~UnitAction() { };
    virtual void Start(GameUnit &unit) = 0;
		virtual void Execute(GameUnit &unit) = 0;
};

#endif