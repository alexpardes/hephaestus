#include "stdafx.h"
//#include "UnitBehavior.h"
//
//Vector2f UnitBehavior::GetSteeringForce(const GameUnit &unit) const {
//	Vector2f seek_force = GetSeekVector(unit);
//	Vector2f avoid_force = GetAvoidVector(unit);
//	Vector2f desired_velocity = /*5.f*avoid_force +*/ seek_force;
//	Vector2f steering_force = desired_velocity - unit.velocity();
//	Util::Scale(steering_force, unit.kMaxAcceleration);
//	return steering_force;
//}
//
//Vector2f UnitBehavior::GetSeekVector(const GameUnit &unit) const {
//	Vector2f desired_velocity = unit.pathing_destination() - unit.position();
//	//float distance = Util::Length(desired_velocity);
//	//float current_speed = Util::Length(unit.velocity());
//	//float braking_distance = 160.f;
//	//if (distance < braking_distance) {
//	//	Util::Scale(desired_velocity,
//	//			distance * unit.kBaseSpeed / braking_distance);
//	//}
//	Util::Scale(desired_velocity, 1.f);
//	return desired_velocity;
//}
//
//Vector2f UnitBehavior::GetAvoidVector(const GameUnit &unit) const {
//	Vector2f desired_velocity(0.f, 0.f);
//	std::list<GameUnit *> nearby_units =
//			game_state_.GetUnitsInCircle(unit.position() + unit.velocity(),
//			unit.kCollisionRadius + Util::Length(unit.velocity()));
//
//	int n_units_avoided = 0;
//	for (std::list<GameUnit *>::iterator nearby_unit = nearby_units.begin();
//			nearby_unit != nearby_units.end(); ++nearby_unit) {
//		if (*nearby_unit != &unit) {
//			Vector2f separation = (*nearby_unit)->position() +
//					(*nearby_unit)->velocity() - unit.position() - unit.velocity();
//			if (Util::Length(separation) <= Util::Length(unit.velocity()) +
//					unit.kCollisionRadius + (*nearby_unit)->kCollisionRadius) {
//				Util::Scale(separation, 1.f - Util::Length(separation) /
//					Util::Length(unit.velocity()));
//				desired_velocity -= separation;
//				++n_units_avoided;
//			}
//		}
//	}
//	if (n_units_avoided) {
//		desired_velocity /= (float) n_units_avoided;
//	}
//	return desired_velocity;
//}
//
//void UnitBehavior::SteerUnit(GameUnit &unit) const {
//	Vector2f steering_force = GetSteeringForce(unit);
//	if (steering_force.x != 0.f || steering_force.y != 0.f) {
//		unit.set_velocity(unit.velocity() + steering_force);
//		if (Util::Length(unit.velocity()) > unit.kBaseSpeed)
//				unit.set_velocity(unit.kBaseSpeed * unit.velocity() /
//				Util::Length(unit.velocity()));
//		unit.set_rotation(Util::FindAngle(Vector2f(1.f, 0.f), unit.velocity()));
//	}
//}