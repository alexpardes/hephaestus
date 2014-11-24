#include "stdafx.h"
#include "MoveAbility.h"
#include "Util.h"
#include "GameUnit.h"
#include "GameState.h"
#include "DirectedSegment.h"
#include "Rectangle.h"
#include <PathFinding/PathFinder.h>
#include <PathFinding/SpatialGraph.h>
#include <PathFinding/SpatialVertex.h>

const float kPathingEpsilon = 1.f;

MoveAbility::MoveAbility(std::shared_ptr<GameUnit> owner,
                         GameState &gameState) : gameState(gameState) {
  this->owner = owner;
}

void MoveAbility::SetDestination(const Vector2f &destination) {
  auto start = gameState.PathingGraph().MakeVertex(owner->Position());
  path = PathFinder::Path(start.get(), gameState.GetWalls(), destination);
}

void MoveAbility::Execute() {
  while (!path.empty() && path.front() == owner->Position()) {
    path.pop_front();
  }

  if (path.empty())
    return;

  Vector2f displacement = path.front() - owner->Position();
  Util::Limit(displacement, owner->Attributes().Speed());

  if (Util::Length(displacement) != 0)
    owner->SetRotation(Util::FindAngle(displacement));

  Vector2f newPosition = owner->Position() + displacement;
  gameState.MoveUnit(owner->Id(), newPosition);
}

bool MoveAbility::DestinationReached() const {
  return path.empty();
}

void MoveAbility::SetPath(const std::deque<const Vector2f> &path) {
  this->path = path;
}