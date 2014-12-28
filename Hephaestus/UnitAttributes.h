#pragma once

#include <string>

class UnitAttributes {
public:
  UnitAttributes(
    int type,
    float collisionRadius,
    float selectionRadius,
    float speed,
    float attackDamage,
    float attackSpeed,
    float attackRange,
    float dispersion,
    float projectileSpeed,
    float maxHealth,
    float healthRegen,
    float regenDelay,
    float moveInstability,
    float rotateInstability,
    float stabilityRecovery,
    float aimWidth) :    
    type(type), speed(speed),    
    collisionRadius(collisionRadius),
    selectionRadius(selectionRadius),
    attackDamage(attackDamage),
    attackSpeed(attackSpeed),
    attackRange(attackRange),
    maxHealth(maxHealth),
    dispersion(dispersion),
    projectileSpeed(projectileSpeed),
    healthRegen(healthRegen),
    regenDelay(regenDelay),
    moveInstability(moveInstability),
    rotateInstability(rotateInstability),
    stabilityRecovery(stabilityRecovery),
    aimWidth(aimWidth) { }

  UnitAttributes() { }

  int Type() const {return type;}

  float Speed() const {return speed;}
  float CollisionRadius() const {return collisionRadius;}
  float SelectionRadius()  const {return selectionRadius;}
  float AttackDamage() const {return attackDamage;}
  float AttackSpeed() const {return attackSpeed;}
  float AttackRange() const {return attackRange;}
  float Dispersion() const {return dispersion;}
  float MaxHealth() const {return maxHealth;}
  float HealthRegen() const {return healthRegen;}
  float RegenDelay() const {return regenDelay;}
  float ProjectileSpeed() const {return projectileSpeed;}
  float MoveInstability() const {return moveInstability;}
  float RotateInstability() const {return rotateInstability;}
  float StabilityRecovery() const {return stabilityRecovery;}
  float AimWidth() const {return aimWidth;}

  void SetSpeed(float speed) {this->speed = speed;}
  void SetCollisionRadius(float collision_radius) {collisionRadius = collision_radius;}
  void SetSelectionRadius(float selection_radius) {selectionRadius = selection_radius;}
  void SetAttackDamage(float attack_damage) {attackDamage = attack_damage;}
  void SetAttackSpeed(float attack_speed) {attackSpeed = attack_speed;}
  void SetDispersion(float dispersion) {this->dispersion = dispersion;}
  void SetAttackRange(float attack_range) {attackRange = attack_range;}
  void SetMaxHealth(float max_health) {maxHealth = max_health;}
  void SetHealthRegen(float healthRegen) {this->healthRegen = healthRegen;}
  void SetProjectileSpeed(float projectileSpeed) {this->projectileSpeed = projectileSpeed;}

private:
  int type;
  float speed, collisionRadius, selectionRadius, attackDamage, attackSpeed,
    attackRange, maxHealth, dispersion, projectileSpeed, healthRegen,
    regenDelay, moveInstability, rotateInstability, stabilityRecovery,
    aimWidth;
};