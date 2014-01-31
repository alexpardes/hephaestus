#pragma once
#include "UnitAbility.h"
#include "Util.h"
#include <deque>

class PathFinder;
class GameState;
class GameUnit;
class Subgoal;

class MoveAbility : public UnitAbility {
  public:
    MoveAbility(GameUnit *owner, PathFinder *pathfinder, GameState *gameState,
        float speed);
    virtual void Execute();
    void SetDestination(const Vector2f &destination);

  private:
    void ApplyForce(Vector2f force, bool useMaxAcceleration);
    Vector2f Seek(const Vector2f &target);
    Vector2f Separate();
    Vector2f SeparateFromUnits();
    Vector2f SeparateFromWalls();
    Vector2f Avoid();
    void Stop();
    Vector2f HandleCollisions();
    Vector2f HandleUnitCollisions(const Vector2f &end);
    Vector2f HandleTerrainCollisions(const Vector2f &end);
    float maxSpeed;
    float acceleration;

    GameUnit *owner;
    PathFinder *pathfinder;
    GameState *gameState;

    std::deque<Subgoal*> path;
    Vector2f *destination;
    Vector2f velocity;
};