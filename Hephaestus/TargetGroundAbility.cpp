#include "stdafx.h"
#include <PathFinding/PathFinder.h>
#include <PathFinding/SpatialGraph.h>
#include <PathFinding/SpatialVertex.h>
#include "GameState.h"
#include "MoveAbility.h"
#include "MinkowskiSum.h"
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

void TargetGroundAbility::SetDestination(const Vector2f &point, bool fullCover) {
  target = point;
  SectorMap sectorMap;
  sectorMap.Create(point, gameState.Walls());
  auto startVertex = gameState.PathingGraph().MakeVertex(owner.Position());
  auto polygonBorder = sectorMap.PolygonBorder();
  if (fullCover)
    polygonBorder = MinkowskiSum::Dilate(polygonBorder, owner.Attributes().CollisionRadius());

  auto path = PathFinder::Path(startVertex.get(), gameState.DilatedWalls(), polygonBorder);
  static_cast<MoveAbility*>(owner.GetAbility("Move"))->SetPath(path);
}

bool TargetGroundAbility::DestinationReached() const {
  return static_cast<MoveAbility*>(owner.GetAbility("Move"))->DestinationReached();
}