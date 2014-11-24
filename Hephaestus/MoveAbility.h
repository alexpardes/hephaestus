#pragma once
#include "UnitAbility.h"
#include "Vector2.h"
#include <deque>

class GameState;
class GameUnit;
class Waypoint;

class MoveAbility : public UnitAbility {
  public:
    MoveAbility(std::shared_ptr<GameUnit> owner, GameState &gameState);
    virtual void Execute();
    virtual std::string Name() const { return "Move"; }
    void SetDestination(const Vector2f &destination);
    bool DestinationReached() const;
    void SetPath(const std::deque<const Vector2f> &path);

  private: 
    void MoveStatic(const Vector2f *currentGoal);

    std::shared_ptr<GameUnit> owner;
    GameState &gameState;
    std::deque<const Vector2f> path;
};