#include "stdafx.h"
#include "GameUnit.h"
#include "UnitAction.h"
#include "Util.h"

GameUnit::GameUnit(UnitId id,
                   const UnitAttributes &attributes,
                   PlayerNumber owner,
                   const Vector2f &position,
                   float rotation) {
	id_ = id;
	is_alive_ = true;
	attributes_ = attributes;
	position_ = position;
	current_health_ = attributes_.max_health();
	owner_ = owner;
	rotation_ = rotation;
  ability = NULL;
  action = NULL;
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
  if (std::abs(unit1.rotation() - unit2.rotation()) > M_PI) {
    rotation_ = a*Util::Angle(unit1.rotation(), M_PI / 2)
        + b*Util::Angle(unit2.rotation(), M_PI / 2);
    rotation_ = Util::Angle(rotation_, 0.f);
  } else {
    	rotation_ = a*unit1.rotation() + b*unit2.rotation();
  }

	id_ = unit1.id();
	name_ = unit1.name();
	owner_ = unit1.owner();
	radius_ = unit1.radius();
}