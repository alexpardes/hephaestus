#ifndef UNIT_ACTION_
#define UNIT_ACTION_

class UnitAbility;

// An action represents a use of an ability. For example a MoveAbility
// contains the logic for moving the unit. A MoveAction passes the
// desired destination to the MoveAbility.
class UnitAction {
	public:
		virtual ~UnitAction() { };
    virtual void Start(UnitAbility *ability) = 0;
		virtual void Execute() = 0;

    // An action's name needs to be the same as its corresponding ability.
    virtual std::string Name() = 0;
};

#endif