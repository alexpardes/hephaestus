#ifndef SIMULATOR_
#define SIMULATOR_

#include "GameState.h"
#include "Pathing.h"
#include "GameInterface.h"

class Simulator {
	public:
		Simulator(GameState &game_state, float timestep);
		void StepSimulation();
		Vector2f GetVelocity(GameUnit &unit);
		std::stack<Vector2f> GetPath(const GameUnit &unit, const Vector2f destination);

	private:
		GameState &game_state_;
		void ProcessUnits(const std::list<std::shared_ptr<GameUnit> > &units);
		void ProcessProjectiles(const std::list<Projectile *> &projectiles);
		const float timestep_;
		Pathfinder pathfinder_;
};

#endif