#include "stdafx.h"
#include "GameUnit.h"
#include "UnitAction.h"

GameUnit::GameUnit(int id,
                   const UnitAttributes &attributes,
                   PlayerNumber owner,
                   const Vector2f &position,
                   float rotation) {
	id_ = id;
	is_alive_ = true;
	attributes_ = attributes;
	position_ = position;
	destination_ = position;
	pathing_destination_ = position;
	current_health_ = attributes_.max_health();
	velocity_ = Vector2f(0.f, 0.f);
	is_stationary_ = true;
	reload_time_ = 0.f;
	target_ = NULL;
	owner_ = owner;
	rotation_ = rotation;
  ability = NULL;
  action = NULL;
}

void GameUnit::ResetReload() {
	reload_time_ = attributes_.attack_speed();
}

float GameUnit::GetHeading() const {
	float heading = rotation_;
	if (pathing_destination() != position_) {
		Vector2f difference = pathing_destination() - position_;
		if (difference.x == 0) {
			if (difference.y > 0) {
				heading = M_PI / 2;
			} else {
				heading = 3 * M_PI / 2;
			}
		} else  {
			heading = std::atanf(difference.y / difference.x);
			if (difference.x < 0) heading += M_PI;
		}
	}
	return heading;
}

void GameUnit::SetAction(UnitAction *action) {
  this->action = action;
  action->Start(ability);
}

void GameUnit::PerformAction() {
  if (action) {
    action->Execute();
  }
}

UnitModel::UnitModel(const GameUnit &unit) {
	current_health_ = unit.current_health();
	position_ = unit.position();
	rotation_ = unit.Rotation();
	id_ = unit.Id();
	name_ = unit.Attributes().name();
	owner_ = unit.Owner();
	radius_ = unit.Attributes().selection_radius();
}

UnitModel::UnitModel(const UnitModel &unit1,
					 const UnitModel &unit2,
					 float weight) {
	float b = weight;
	float a = 1 - weight;
	current_health_ = a*unit1.current_health() + b*unit2.current_health();
	position_ = a*unit1.position() + b*unit2.position();
	rotation_ = a*unit1.rotation() + b*unit2.rotation();
	id_ = unit1.id();
	name_ = unit1.name();
	owner_ = unit1.owner();
	radius_ = unit1.radius();
}