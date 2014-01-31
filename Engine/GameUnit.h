#pragma once

#include "Vector2.h"

class GameUnit {
  public:
    GameUnit(const Vector2 &location);
  private:
    Vector2 location;
};