#include "stdafx.h"
#include "GameUnit.h"
#include "HealthRegenAbility.h"

HealthRegenAbility::HealthRegenAbility(GameUnit &owner, float regenSpeed) :
  owner(owner), regenSpeed(regenSpeed) { }

void HealthRegenAbility::Execute() {
  // TODO: Inject frame time or don't use it.
  if (50.f * owner.TurnsSinceHit() > owner.Attributes().RegenDelay())
    owner.ModifyHealth(regenSpeed * owner.Attributes().MaxHealth());
}