#pragma once

#include "UnitAction.h"
#include "GameUnit.h"

class AttackAbility;
class MoveAbility;

class AttackAction : public UnitAction {
  public:
    AttackAction(GameUnit &target) : target(target) { }

    virtual void Start(GameUnit &unit);
    virtual void Execute(GameUnit &unit);
    virtual std::string Name();

  private:
    GameUnit &target;
    MoveAbility *moveAbility;
    AttackAbility *attackAbility;
};