#pragma once

#include "UnitAction.h"
#include "GameUnit.h"

class AttackAbility;
class MoveAbility;

class AttackAction : public UnitAction {
  public:
    AttackAction(UnitId target) : target(target) { }

    virtual void Start(GameUnit &unit);
    virtual void Execute(GameUnit &unit);
    virtual std::string Name();

  private:
    UnitId target;
    MoveAbility *moveAbility;
    AttackAbility *attackAbility;
};