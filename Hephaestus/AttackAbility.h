#pragma once
#include "UnitAbility.h"
#include "GameUnit.h"

class GameState;
class MoveAbility;

class AttackAbility : public UnitAbility {
  public:
    AttackAbility(std::shared_ptr<GameUnit> owner, GameState *gameState,
        float damage, float dispersion, float speed, float projectileSpeed, float range);

    virtual void Execute();
    virtual void ExecutePassive();
    virtual std::string Name() const { return "Attack"; }
    void SetTarget(UnitId target) { this->target = target; }
    void SetTargetPoint(const Vector2f &point);
    const std::shared_ptr<GameUnit> Target() const;

    void EnableMovement(bool enableMovement) {
      movementEnabled = enableMovement;
    }

    bool CanAttack(UnitId target) const;

  private:
    void Attack(const GameUnit &unit);
    void Rotate(float angle, bool isStrafing) const;
    bool IsInRange(const GameUnit &unit) const;
    bool IsInRange() const;

    std::shared_ptr<GameUnit> owner;
    MoveAbility *moveAbility;
    GameState *gameState;
    UnitId target;
    Vector2f targetPoint;
    float damage, dispersion, projectileSpeed, speed, range, loadTime;
    bool movementEnabled, useTargetPoint;
};