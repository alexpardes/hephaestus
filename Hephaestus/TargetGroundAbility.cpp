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
}

void TargetGroundAbility::SetDestination(const Vector2f &point) {
  SectorMap sectorMap;
  sectorMap.Create(point, gameState.GetWalls());
  auto startVertex = gameState.PathingGraph().MakeVertex(owner.Position());
  auto path = PathFinder::Path(startVertex.get(), gameState.PathingGraph().DilatedWalls(), sectorMap.PolygonBorder());
  static_cast<MoveAbility*>(owner.GetAbility("Move"))->SetPath(path);
}

bool TargetGroundAbility::DestinationReached() const {
  return static_cast<MoveAbility*>(owner.GetAbility("Move"))->DestinationReached();
}