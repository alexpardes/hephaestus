#pragma once
#include "UnitAbility.h"
#include "GameUnit.h"

class GameState;
class MoveAbility;

class AttackAbility : public UnitAbility {
  public:
    AttackAbility(std::shared_ptr<GameUnit> owner, GameState *gameState,
        float damage, float speed, float range);

    virtual void Execute();
    virtual std::string Name() { return "Attack"; }
    void SetTarget(UnitId target) { this->target = target; }

  private:
    bool IsInRange() const;
    bool IsUnobstructed() const;
    std::shared_ptr<GameUnit> owner;
    MoveAbility *moveAbility;
    GameState *gameState;
    UnitId target;
    float damage, speed, range, loadTime;
};