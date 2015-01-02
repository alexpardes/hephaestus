#pragma once
#include "UnitAbility.h"

class GameUnit;

class HealthRegenAbility : public UnitAbility {
  public:
    HealthRegenAbility(GameUnit &owner, float regenSpeed);
    virtual void ExecutePassive();
    virtual std::string Name() const { return "HealthRegen"; }

  private:
    GameUnit &owner;
    float regenSpeed;
};