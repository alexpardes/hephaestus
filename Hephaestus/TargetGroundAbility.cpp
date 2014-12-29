#include "stdafx.h"
#include <PathFinding/PathFinder.h>
#include <PathFinding/SpatialGraph.h>
#include <PathFinding/SpatialVertex.h>
#include "GameState.h"
#include "MoveAbility.h"
#include "TargetGroundAbility.h"

TargetGroundAbility::TargetGroundAbility(GameUnit &owner, GameState &gameState) :
    owner(owner), gameState(gameState) { }

std::string TargetGroundAbility::Name() const {
  return "TargetGround";
}

void TargetGroundAbility::Execute() {
  auto moveAbility = static_cast<MoveAbility*>(owner.GetAbility("Move"));
  moveAbility->Execute();
  if (DestinationReached()) {
    auto v = target - owner.Position();
    owner.SetRotation(Util::FindAngle(v));
  }
}

void TargetGroundAbility::SetDestination(const Vector2f &point) {
  target = point;
  SectorMap sectorMap;
  sectorMap.Create(point, gameState.Walls());
  auto startVertex = gameState.PathingGraph().MakeVertex(owner.Position());
  auto path = PathFinder::Path(startVertex.get(), gameState.DilatedWalls(), sectorMap.PolygonBorder());
  static_cast<MoveAbility*>(owner.GetAbility("Move"))->SetPath(path);
}

bool TargetGroundAbility::DestinationReached() const {
  return static_cast<MoveAbility*>(owner.GetAbility("Move"))->DestinationReached();
}