#pragma once
#include "UnitAction.h"
#include "AttackAction.h"
#include "MoveAction.h"
#include <memory>

class AttackAction;
class AttackMoveAbility;
class GameUnit;

class AttackMoveAction : public UnitAction {
  public:
    AttackMoveAction(const Vector2f &destination) :
        destination(destination) { }

    virtual void Start(GameUnit &unit);
    virtual void Execute(GameUnit &unit);
    virtual bool IsFinished() const;

  private:
    Vector2f destination;
    AttackMoveAbility *ability;
};