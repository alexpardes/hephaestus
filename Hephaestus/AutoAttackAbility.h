#pragma once
#include "UnitAbility.h"
#include <memory>

class GameState;
class GameUnit;
class AttackAbility;

class AutoAttackAbility : public UnitAbility {
  public:
    AutoAttackAbility(GameUnit &owner, GameState &gameState);
    virtual void Execute();
    virtual std::string Name() { return "AutoAttack"; }

  private:
    // TODO: these fields can probably be moved to base class.
    GameUnit &owner;
    GameState &gameState;

    std::shared_ptr<GameUnit> target;
    AttackAbility *attackAbility;
};