#pragma once

#include "Vector2.h"
#include "GameUnit.h"

typedef std::vector<GameUnit *> UnitList;

class Game {
	public:
		void AddUnit(const Vector2 &location);

  private:
    UnitList units;
};