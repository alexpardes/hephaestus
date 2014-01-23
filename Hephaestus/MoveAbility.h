#pragma once
#include "UnitAbility.h"
#include "Util.h"
#include <deque>

class PathFinder;
class GameState;
class GameUnit;

class MoveAbility : public UnitAbility {
  public:
    MoveAbility(GameUnit *owner, PathFinder *pathfinder, GameState *gameState,
        float speed);
    virtual void Execute();
    void SetDestination(const Vector2f &destination);

  private:
    void ApplyForce(Vector2f force, bool useMaxAcceleration);
    void Seek(const Vector2f &target);
    void Stop();
    float maxSpeed;
    float acceleration;

    GameUnit *owner;
    PathFinder *pathfinder;
    GameState *gameState;

    std::deque<const Vector2f> path;
    Vector2f velocity;
};