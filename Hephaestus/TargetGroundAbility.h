#pragma once
#include "UnitAbility.h"

class GameState;
class GameUnit;
class SpatialVertex;

class TargetGroundAbility : public UnitAbility {
  public:
    TargetGroundAbility(GameUnit &owner, GameState &gameState);
    virtual void Execute();
    virtual std::string Name() const;
    void SetDestination(const Vector2f &point);

  private:
    GameUnit &owner;
    GameState &gameState;
};