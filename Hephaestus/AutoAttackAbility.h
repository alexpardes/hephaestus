#pragma once
#include "UnitAbility.h"
#include "Vector2.h"
#include <memory>

class GameState;
class GameUnit;
class AttackAbility;

class AutoAttackAbility : public UnitAbility {
  public:
    AutoAttackAbility(GameUnit &owner, GameState &gameState);
    virtual void Execute();
    virtual std::string Name() const { return "AutoAttack"; }
    bool HasTarget() const { return target != nullptr; }
    void SetTargetPoint(const Vector2f &point);

  private:
    // TODO: these fields can probably be moved to base class.
    GameUnit &owner;
    GameState &gameState;

    std::shared_ptr<GameUnit> target;
    bool useTargetPoint;
    Vector2f targetPoint;
    AttackAbility *attackAbility;
};