#include "stdafx.h"
#include "GameUnit.h"
#include "HealthRegenAbility.h"

HealthRegenAbility::HealthRegenAbility(GameUnit &owner, float regenSpeed) :
  owner(owner), regenSpeed(regenSpeed) { }

void HealthRegenAbility::Execute() {
  owner.ModifyHealth(regenSpeed * owner.Attributes().MaxHealth());
}