#pragma once
#include "UnitAbility.h"
#include "Vector2.h"

class GameState;
class GameUnit;
class AttackAbility;
class MoveAbility;

class AttackMoveAbility : public UnitAbility {
  public:
    AttackMoveAbility(GameUnit &owner, GameState &gameState,
        float aggroRange, float deaggroRange);

    virtual void Execute();
    virtual std::string Name() { return "AttackMove"; }
    void SetDestination(const Vector2f &destination);

  private:
    GameUnit &owner;
    GameState &gameState;
    float aggroRange, deaggroRange;
    Vector2f destination;
    std::shared_ptr<GameUnit> target;
    AttackAbility *attackAbility;
    MoveAbility *moveAbility;
};