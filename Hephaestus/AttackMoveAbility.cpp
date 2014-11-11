#include "stdafx.h"
#include "AttackMoveAbility.h"
#include "GameUnit.h"
#include "AutoAttackAbility.h"
#include "MoveAbility.h"
#include "Util.h"
#include "GameState.h"

AttackMoveAbility::AttackMoveAbility(GameUnit &owner,
                                     GameState &gameState) :
    owner(owner), gameState(gameState) {

  autoAttackAbility = dynamic_cast<AutoAttackAbility*>(owner.GetAbility("AutoAttack"));
  moveAbility = dynamic_cast<MoveAbility*>(owner.GetAbility("Move"));
}

void AttackMoveAbility::SetDestination(const Vector2f &destination) {
  this->destination = destination;
  moveAbility->SetDestination(destination);
}

void AttackMoveAbility::Execute() {
  autoAttackAbility->Execute();
  if (!autoAttackAbility->HasTarget())
    moveAbility->Execute();
}


bool AttackMoveAbility::DestinationReached() const {
  return moveAbility->DestinationReached();
}