#pragma once
#include "UnitAbility.h"
#include "Vector2.h"

class GameState;
class GameUnit;
class AutoAttackAbility;
class MoveAbility;

class AttackMoveAbility : public UnitAbility {
  public:
    AttackMoveAbility(GameUnit &owner, GameState &gameState);

    virtual void Execute();
    virtual std::string Name() const { return "AttackMove"; }
    void SetDestination(const Vector2f &destination);
    bool DestinationReached() const;

  private:
    GameUnit &owner;
    GameState &gameState;
    Vector2f destination;
    AutoAttackAbility *autoAttackAbility;
    MoveAbility *moveAbility;
};