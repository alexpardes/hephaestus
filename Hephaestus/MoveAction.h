#pragma once

#include "UnitAction.h"
#include "Vector2.h"

class MoveAbility;

class MoveAction : public UnitAction {
	public:
		MoveAction(const Vector2f &location) {
      this->location = location;
    }

    virtual void Start(GameUnit &unit);
    virtual void Execute(GameUnit &unit);
    virtual bool IsFinished() const;
    virtual std::string Name() const;

	private:
		Vector2f location;
    MoveAbility *ability;
};