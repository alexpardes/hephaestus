#pragma once

#include <string>

class UnitAttributes {
public:
  UnitAttributes(int type, float collisionRadius,
    float selectionRadius, float speed,
    float attackDamage,
    float attackSpeed, float attackRange,
    float maxHealth) : type(type), speed(speed),
    collisionRadius(collisionRadius),
    selectionRadius(selectionRadius),
    attackDamage(attackDamage), attackSpeed(attackSpeed),
    attackRange(attackRange), maxHealth(maxHealth) { }
  UnitAttributes() { }

  int Type() const {return type;}

  float Speed() const {return speed;}
  float CollisionRadius() const {return collisionRadius;}
  float SelectionRadius()  const {return selectionRadius;}
  float AttackDamage() const {return attackDamage;}
  float AttackSpeed() const {return attackSpeed;}
  float AttackRange() const {return attackRange;}
  float MaxHealth() const {return maxHealth;}

  void SetSpeed(float speed) {this->speed = speed;}
  void SetCollisionRadius(float collision_radius) {collisionRadius = collision_radius;}
  void SetSelectionRadius(float selection_radius) {selectionRadius = selection_radius;}
  void SetAttackDamage(float attack_damage) {attackDamage = attack_damage;}
  void SetAttackSpeed(float attack_speed) {attackSpeed = attack_speed;}
  void SetAttackRange(float attack_range) {attackRange = attack_range;}
  void SetMaxHealth(float max_health) {maxHealth = max_health;}

private:
  int type;
  float speed, collisionRadius, selectionRadius, attackDamage,
    attackSpeed, attackRange, maxHealth;
};