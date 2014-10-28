#pragma once
#include "UnitAbility.h"
#include "Vector2.h"
#include <deque>

class PathFinder;
class GameState;
class GameUnit;
class Waypoint;

class MoveAbility : public UnitAbility {
  public:
    MoveAbility(std::shared_ptr<GameUnit> owner, PathFinder *pathfinder, GameState *gameState,
        float speed);
    virtual void Execute();
    virtual std::string Name() { return "Move"; }
    void SetDestination(const Vector2f &destination);

  private:
    void ApplyForce(Vector2f &force, bool useMaxAcceleration);
    void ScaleForce(Vector2f &force, float distance) const;
    Vector2f Seek(const Vector2f &target);
    Vector2f Separate() const;
    Vector2f SeparateFromUnits() const;
    Vector2f SeparateFromWalls() const;
    Vector2f Avoid() const;
    Vector2f AvoidUnits() const;
    Vector2f AvoidWalls() const;
    Vector2f AvoidForce(const Vector2f &point, int degree = 2) const;
    void Stop();
    Vector2f HandleCollisions();
    float maxSpeed;
    float acceleration;

    std::shared_ptr<GameUnit> owner;
    PathFinder *pathfinder;
    GameState *gameState;

    std::deque<Waypoint*> path;
    Vector2f *destination;
    Vector2f velocity;
};