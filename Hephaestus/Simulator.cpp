#include "stdafx.h"
#include "Simulator.h"

Simulator::Simulator(GameState &game_state,
					 float timestep) :
		game_state_(game_state),
		timestep_(timestep),
		pathfinder_(game_state) { }

void Simulator::StepSimulation() {
	ProcessProjectiles(game_state_.projectiles());
	ProcessUnits(game_state_.units());
}

void Simulator::ProcessProjectiles(const std::list<Projectile *> &projectiles) {
	std::list<Projectile *> dead_projectiles;
	for (std::list<Projectile *>::const_iterator projectile =
			projectiles.begin(); projectile != projectiles.end();
			++projectile) {
		if (!(*projectile)->target()->is_alive()) {
			dead_projectiles.push_back(*projectile);
		} else {
			Vector2f trajectory = (*projectile)->target()->position() -
					(*projectile)->position();
			float travel_distance = timestep_ * (*projectile)->kSpeed;
			if (Util::Length(trajectory) <=
					travel_distance +
					(*projectile)->target()->attributes().collision_radius()) {
				(*projectile)->target()->
						modify_health(-(*projectile)->damage());
				dead_projectiles.push_back(*projectile);
			} else {
				Util::Scale(trajectory, travel_distance);
				(*projectile)->Move(trajectory);
			}
		}
	}
	for (std::list<Projectile *>::iterator projectile_iterator =
			dead_projectiles.begin();
			projectile_iterator != dead_projectiles.end();
			++projectile_iterator) {
		game_state_.RemoveProjectile(*projectile_iterator);
	}
}

void Simulator::ProcessUnits(const std::list<GameUnit *> &units) {
	std::list<GameUnit *> dead_units;
	for (std::list<GameUnit *>::const_iterator unit_iterator = units.begin();
			unit_iterator != units.end(); ++unit_iterator) {
		GameUnit &unit = **unit_iterator;
		if (!unit.is_alive()) {
			dead_units.push_back(*unit_iterator);
		} else if (unit.current_health() <= 0) {
			unit.Kill();
		} else {
			if (unit.target()) {
				if (!unit.target()->is_alive()) {
					unit.set_target(NULL);
					unit.set_destination(unit.position());
					unit.ClearPath();
				} else if (unit.target()->position()
						!= unit.destination()) {
					unit.set_destination(unit.target()->position());
					unit.ClearPath();
				}
			}
			if (/*unit.pathing_destination() ==
					unit.position() &&*/
					unit.destination() !=
					unit.position()) {
				float distance = 0.f;
				if (unit.target()) {
					distance = unit.attributes().attack_range() +
							unit.target()->attributes().collision_radius();
				}
				pathfinder_.PathUnit(unit, distance);
			}
			if (unit.pathing_destination() !=
					unit.position()) {
				//MoveUnit(*unit_iterator);
				//behavior_.SteerUnit(unit);
				game_state_.RemoveFromPathingGrid(unit);
				float movement_distance = unit.attributes().speed() * timestep_;
				Vector2f new_position = unit.position();
				while (unit.pathing_destination() != unit.position()) {
					float waypoint_distance = Util::Distance(new_position,
							unit.pathing_destination());
					if (waypoint_distance >= movement_distance) {
						Vector2f movement = unit.pathing_destination() -
								unit.position();
						Util::Scale(movement, unit.attributes().speed() * timestep_);
						new_position = unit.position() + movement;
						break;
					} else {
						new_position = unit.pathing_destination();
						unit.pop_path();
						movement_distance -= waypoint_distance;
					}
				}
				unit.set_rotation(unit.GetHeading());
				unit.set_position(new_position);
				/*unit.set_position(unit.position() +
						timestep_ * unit.velocity());*/
				game_state_.AddToPathingGrid(unit);
				if (Util::Distance(unit.position(),
						unit.pathing_destination()) <= 10.f) {
					unit.pop_path();
					if (unit.pathing_destination() ==
							unit.position()) {
						unit.set_is_stationary(true);
						unit.set_destination(unit.position());
						//game_state_.AddToPathingGrid(*unit_iterator);
					}
				}
				game_state_.UpdateUnitGrid(unit);
			}
			if (!unit.IsLoaded()) {
				unit.ReduceReloadTime(timestep_);
			}
			if (unit.target()) {
				if (unit.IsLoaded() &&
						Util::Distance(unit.target()->position(),
						unit.position()) <=
						unit.attributes().attack_range() +
						unit.target()->attributes().collision_radius()) {
					game_state_.AddProjectile(unit.position(),
							unit.target(),
							unit.attributes().attack_damage());
					unit.ResetReload();
				}
			}
		}
	}
	for (std::list<GameUnit *>::iterator unit_iterator = dead_units.begin();
			unit_iterator != dead_units.end(); ++unit_iterator) {
		game_state_.RemoveUnit(*unit_iterator);
	}
}

//void Simulator::MoveUnit(GameUnit &unit) {
//	unit.set_rotation(unit.GetHeading());
//
//	//unit.set_position(unit.position() + unit.velocity() * timestep_);
//
//	if (Util::Distance(unit.position(), unit.pathing_destination()) <=
//			unit.speed() * timestep_) {
//		unit.set_position(unit.pathing_destination());
//		unit.pop_path();
//		if (unit.pathing_destination() == unit.position()) {
//			unit.set_is_stationary(true);
//			unit.set_destination(unit.position());
//			game_state_.AddToPathingGrid(unit);
//		}
//	} else {
//		unit.set_position(unit.position() + GetVelocity(unit) * timestep_);
//	}
//	game_state_.UpdateUnitGrid(unit);
//}
//
//Vector2f Simulator::GetVelocity(GameUnit &unit) {
//	return Vector2f(unit.speed() * std::cosf(unit.rotation()),
//						unit.speed() * std::sinf(unit.rotation()));
//}