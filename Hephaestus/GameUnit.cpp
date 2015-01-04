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
  stability = 1.f;
  turnsSinceHit = 0;
	currentHealth = attributes.MaxHealth();
	this->owner = owner;
	this->rotation = rotation;
  torsoRotation = 0.f;
  previousRotation = rotation;
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

float GameUnit::TorsoRotation() const {
  return rotation + torsoRotation;
}

float GameUnit::RelativeTorsoRotation() const {
  return torsoRotation;
}

void GameUnit::SetTorsoRotation(float rotation) {
  torsoRotation = rotation - this->rotation;
}

void GameUnit::SetRelativeTorsoRotation(float rotation) {
  torsoRotation = rotation;
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

float GameUnit::Stability() const {
  return stability;
}

void GameUnit::PerformAction() {
  auto movementInstability = attributes.MoveInstability() * Util::Distance(position, previousPosition) / attributes.Speed();
  auto rotationInstability = std::min(1.f, attributes.RotateInstability() * Util::AngleBetween(rotation, previousRotation));
  auto newStability = (1.f - movementInstability) * (1.f - rotationInstability);
  if (newStability < stability)
    stability = newStability;
  else
    stability = attributes.StabilityRecovery() * newStability + (1 - attributes.StabilityRecovery()) * stability;

  previousPosition = position;
  previousRotation = rotation;
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

  for (auto ability : abilities) {
    ability.second->ExecutePassive();
  }

  ++turnsSinceHit;
}

int GameUnit::TurnsSinceHit() const {
  return turnsSinceHit;
}

void GameUnit::OnAttacked(const Projectile &projectile) {
  auto damage = projectile.CalculateDamage(*this);
  ModifyHealth(-damage);
  turnsSinceHit = 0;

  auto damageFraction = damage / currentHealth;
  if (damageFraction > 0.1f && !action) {
    SetAction(new TargetGroundAction(projectile.Origin(), true));
  } else if (damageFraction > 0.05 && !action) {
    SetAction(new TargetGroundAction(projectile.Origin(), false));
  }
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
  torsoRotation = unit.TorsoRotation();
	id = unit.Id();
	type = unit.Attributes().Type();
	owner = unit.Owner();
	radius = unit.Attributes().SelectionRadius();
  stability = unit.Stability();  
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
  stability = a * unit1.Stability() + b * unit2.Stability();
	position = a*unit1.Position() + b*unit2.Position();
  rotation = Util::InterpolateAngles(unit1.Rotation(), unit2.Rotation(), weight);
  torsoRotation = Util::InterpolateAngles(unit1.TorsoRotation(), unit2.TorsoRotation(), weight);

  maxHealth = unit2.MaxHealth();
	id = unit1.Id();
	type = unit1.Type();
	owner = unit1.Owner();
	radius = unit1.Radius();
  visibility = unit2.Visibility();
  isVisible = unit2.isVisible;
  facing = unit2.facing;
}