#ifndef UNITBEHAVIOR_
#define UNITBEHAVIOR_

#include "GameState.h"
#include "GameUnit.h"
#include "Util.h"

class UnitBehavior {
	public:
		UnitBehavior(GameState &game_state) : game_state_(game_state) { }
		void SteerUnit(GameUnit &unit) const;
	private:
		Vector2f GetSteeringForce(const GameUnit &unit) const;
		Vector2f GetSeekVector(const GameUnit &unit) const ;
		Vector2f GetAvoidVector(const GameUnit &unit) const;
		GameState &game_state_;
};

#endif