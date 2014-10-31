#include "stdafx.h"
#include "GameUnit.h"
#include "UnitAction.h"
#include "UnitAbility.h"
#include "Util.h"

GameUnit::GameUnit(UnitId id,
                   const UnitAttributes &attributes,
                   PlayerNumber owner,
                   const Vector2f &position,
                   float rotation) {
	id_ = id;
	is_alive_ = true;
	attributes_ = attributes;
	this->position = position;
	current_health_ = attributes_.MaxHealth();
	owner_ = owner;
	rotation_ = rotation;
  action = NULL;
}

void GameUnit::SetAction(UnitAction *action) {
  if (action) {
    this->action = action;
    action->Start(*this);
  }
}

void GameUnit::AddAbility(UnitAbility *ability) {
  abilities[ability->Name()] = ability;
}

UnitAbility *GameUnit::GetAbility(const std::string &name) {
  UnitAbility *result = NULL;
  if (abilities.count(name)) {
    result = abilities[name];
  }
  return result;
}

void GameUnit::PerformAction() {
  if (current_health_ <= 0) {
    is_alive_ = false;
  } else if (action) {
    action->Execute(*this);
  } else if (idleAbility) {
    idleAbility->Execute();
  }
}

UnitModel::UnitModel(const GameUnit &unit) {
	current_health_ = unit.CurrentHealth();
  maxHealth = unit.Attributes().MaxHealth();
	position_ = unit.Position();
	rotation_ = unit.Rotation();
	id_ = unit.Id();
	name = unit.Attributes().name();
	owner_ = unit.Owner();
	radius_ = unit.Attributes().selection_radius();
  //sightMap = new SectorMap(unit.SightMap());
  visibility = unit.Visibility();
  isVisible = true;
}

UnitModel::UnitModel(const UnitModel &unit1,
                     const UnitModel &unit2,
                     float weight) {
	float b = weight;
	float a = 1 - weight;
	current_health_ = a*unit1.CurrentHealth() + b*unit2.CurrentHealth();
  maxHealth = unit2.MaxHealth();
	position_ = a*unit1.position() + b*unit2.position();
  rotation_ = Util::InterpolateAngles(unit1.rotation(),
      unit2.rotation(), weight);

	id_ = unit1.Id();
	name = unit1.Name();
	owner_ = unit1.Owner();
	radius_ = unit1.Radius();
  sightMap = unit2.sightMap;
  visibility = unit2.Visibility();
  isVisible = unit2.isVisible;
}