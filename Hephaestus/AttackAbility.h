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
    const std::shared_ptr<GameUnit> Target() const;

    void EnableMovement(bool enableMovement) {
      movementEnabled = enableMovement;
    }

    bool CanAttack(UnitId target) const;

  private:
    void Attack(const GameUnit &unit);

    void ChangeAttackPoint();
    Vector2f AttackPoint() const;
    Vector2f LeftAttackPoint() const;
    Vector2f RightAttackPoint() const;

    // Returns rotated by the unit's rotation and translated by the unit's
    // position.
    Vector2f ApplyUnitTransform(const Vector2f& v) const;
    bool IsInRange(const GameUnit &unit) const;
    bool IsInRange() const;

    std::vector<Vector2f> UnobstructedPointsOnTarget(UnitId target,
        const Vector2f &attackPoint) const;
    std::vector<Vector2f> UnobstructedPointsOnTarget(
        const Vector2f &attackPoint) const;
    std::shared_ptr<GameUnit> owner;
    MoveAbility *moveAbility;
    GameState *gameState;
    UnitId target;
    float damage, speed, range, loadTime;
    bool movementEnabled;

    // These are the locations that the unit's projectile can spawn from when
    // the unit has position = (0, 0) and rotation = 0.
    Vector2f leftAttackOffset, rightAttackOffset;
    bool usingLeftAttackPoint;    
};