#include "stdafx.h"
#include "Circle.h"
#include "GameUnit.h"
#include "Projectile.h"
#include "TargetGroundAction.h"
#include "UnitAction.h"
#include "UnitAbility.h"
#include "Util.h"

GameUnit::GameUnit(UnitId id,
                   const UnitAttributes &attributes,
                   PlayerNumber owner,
                   const Vector2f &position,
                   float rotation) {
	this->id = id;
	isAlive = true;
	this->attributes = attributes;
	this->position = position;
  previousPosition = position;
	currentHealth = attributes.MaxHealth();
	this->owner = owner;
	this->rotation = rotation;
  action = nullptr;
  facing = kRight;
}

size_t GameUnit::HashCode() const {
  size_t hash = GameObject::HashCode();
  Util::Hash(hash, isAlive);
  Util::Hash(hash, currentHealth);
  Util::Hash(hash, facing);
  Util::Hash(hash, id);
  Util::Hash(hash, owner);
  return hash;
}

Vector2f GameUnit::PreviousPosition() const {
  return previousPosition;
}

void GameUnit::SetAction(UnitAction *action) {
  this->action = action;
  if (action)
    action->Start(*this);
}

void GameUnit::AddAbility(UnitAbility *ability) {
  abilities[ability->Name()] = ability;
}

UnitAbility *GameUnit::GetAbility(const std::string &name) {
  UnitAbility *result = nullptr;
  if (abilities.count(name)) {
    result = abilities[name];
  }
  return result;
}

void GameUnit::AddPassiveAbility(UnitAbility *ability) {
  passiveAbilities.push_back(ability);
}

void GameUnit::PerformAction() {
  previousPosition = position;
  if (currentHealth <= 0) {
    isAlive = false;
    return;
  }
  
  if (action) {
    if (action->IsFinished()) {
      action = nullptr;
    } else {
    action->Execute(*this);
    }
  }
  
  if (!action && idleAbility) {
    idleAbility->Execute();
  }

  for (auto ability : passiveAbilities) {
    ability->Execute();
  }
}

void GameUnit::OnAttacked(const Projectile &projectile) {
  ModifyHealth(-projectile.CalculateDamage(*this));
  if (!action)
    SetAction(new TargetGroundAction(projectile.Origin()));
}

void GameUnit::ModifyHealth(float health) {
  currentHealth += health;
  if (currentHealth < 0)
    currentHealth = 0;

  if (currentHealth > attributes.MaxHealth())
    currentHealth = attributes.MaxHealth();
}

LineSegment GameUnit::SegmentFromUnit(const Vector2f &viewPoint) const {
  Circle circle(Position(), Attributes().CollisionRadius());
  auto widestPoints = circle.WidestPoints(viewPoint);
  return LineSegment(widestPoints.first, widestPoints.second);
}

UnitModel::UnitModel(const GameUnit &unit) {
	currentHealth = unit.CurrentHealth();
  maxHealth = unit.Attributes().MaxHealth();
	position = unit.Position();
	rotation = unit.Rotation();
	id = unit.Id();
	type = unit.Attributes().Type();
	owner = unit.Owner();
	radius = unit.Attributes().SelectionRadius();
  
  visibility = unit.SightMap().ToPolygon();
  facing = unit.Facing();
  isVisible = true;
}

UnitModel::UnitModel(const UnitModel &unit1,
                     const UnitModel &unit2,
                     float weight) {
	float b = weight;
	float a = 1 - weight;
	currentHealth = a*unit1.CurrentHealth() + b*unit2.CurrentHealth();
  maxHealth = unit2.MaxHealth();
	position = a*unit1.Position() + b*unit2.Position();
  rotation = Util::InterpolateAngles(unit1.Rotation(),
      unit2.Rotation(), weight);

	id = unit1.Id();
	type = unit1.Type();
	owner = unit1.Owner();
	radius = unit1.Radius();
  visibility = unit2.Visibility();
  isVisible = unit2.isVisible;
  facing = unit2.facing;
}