#ifndef SIMULATOR_
#define SIMULATOR_

#include "Commands.h"
#include "GameState.h"
#include "Pathing.h"
#include "GameInterface.h"

class Simulator {
	public:
		Simulator(GameState &game_state, float timestep);
		void StepSimulation();
		Vector2f GetVelocity(GameUnit &unit);

	private:
		GameState &game_state_;
		void ProcessUnits(const std::list<GameUnit *> &units);
		void ProcessProjectiles(const std::list<Projectile *> &projectiles);
		void MoveUnit(GameUnit &unit);
		const float timestep_;
		Pathfinder pathfinder_;
};

#endif