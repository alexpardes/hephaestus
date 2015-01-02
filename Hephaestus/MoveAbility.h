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
    virtual void ExecutePassive();
    virtual std::string Name() const { return "Move"; }
    void SetDestination(const Vector2f &destination);
    bool DestinationReached() const;
    void SetPath(const std::deque<const Vector2f> &path);

  private:
    /* Returns the top speed that the unit can be traveling at the start of the
     * given path leg (segment) without going off the path. */
    float MaxSpeedAtLegStart(int legIndex) const;
    float MaxAcceleration() const;

    std::shared_ptr<GameUnit> owner;
    GameState &gameState;
    std::deque<const Vector2f> path;
    Vector2f velocity;
};