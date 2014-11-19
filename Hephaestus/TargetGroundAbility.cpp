#include "stdafx.h"
#include <PathFinding/PolygonPathFinder.h>
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
  //if (moveAbility->DestinationReached() && !path.empty()) {
  //  moveAbility->SetDestination(path.front()->Point());
  //  path.pop_front();
  //}
  moveAbility->Execute();
}

void TargetGroundAbility::SetDestination(const Vector2f &point) {
  SectorMap sectorMap;
  sectorMap.Create(point, gameState.GetWalls());
  auto startVertex = gameState.PathingGraph().MakeVertex(owner.Position(), owner.SightMap());
  auto path = PolygonPathFinder::Path(startVertex.get(), gameState.GetWalls(), sectorMap.PolygonBorder());
  static_cast<MoveAbility*>(owner.GetAbility("Move"))->SetPath(path);
}