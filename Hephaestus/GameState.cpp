#include "stdafx.h"
#include "GameState.h"
#include "Util.h"
#include "MoveAbility.h"
#include "AttackAbility.h"
#include "AttackMoveAbility.h"
#include "AutoAttackAbility.h"
#include "HealthRegenAbility.h"
#include "SectorMap.h"
#include "TargetGroundAbility.h"
#include "Timer.h"
#include <PathFinding/SpatialGraph.h>

//const float GameState::kPathingResolution = 25.f;
const float GameState::kUnitGridResolution = 8.f;

GameState::GameState(const std::vector<const UnitAttributes> &unitDefinitions, 
                     const Vector2i &mapSize,
                     SpatialGraph *pathingGraph) : 
    unitDefinitions(unitDefinitions) {

  this->pathingGraph = pathingGraph;
	maxUnitRadius = 0.f;
	this->mapSize = mapSize;
	Vector2f mapPixelSize = Util::ToVector2f(mapSize);

	unitGridWidth = (int) std::ceilf(mapPixelSize.x / kUnitGridResolution);
	unitGridHeight = (int) std::ceilf(mapPixelSize.y / kUnitGridResolution);
  unitGrid = UnitGrid(unitGridWidth);  
	for (int i = 0; i < unitGridWidth; ++i) {
    unitGrid[i] = std::vector<std::list<std::shared_ptr<GameUnit>>>(unitGridHeight);
	}

  lastUnitId = 0;
}

GameState::~GameState() {
	for (std::list<Projectile *>::iterator projectile = projectiles.begin();
			projectile != projectiles.end(); ++projectile) {
		delete *projectile;
	}
}

Vector2f GameState::GetUnitPosition(UnitId id) const {
  return GetUnit(id)->Position();
}

void GameState::MoveUnit(UnitId id, Vector2f location) {
  assert(location.x >= 0 && location.x <= mapSize.x);
  assert(location.y >= 0 && location.y <= mapSize.y);

  std::shared_ptr<GameUnit> unit = GetUnit(id);
  Vector2f previousPosition = unit->Position();
  unit->SetPosition(Constrain(location));
  UpdateUnitGrid(unit, previousPosition);
}

Vector2f GameState::Constrain(Vector2f location) const {
  if (location.x < 0) {
    location.x = 0.f;
  } else if (location.x > map_size().x) {
    location.x = (float) map_size().x;
  }
  if (location.y < 0) {
    location.y = 0.f;
  } else if (location.y > map_size().y) {
    location.y = (float) map_size().y;
  }
  return location;
}

//TODO: remove duplication; don't modify lists during iteration.
void GameState::ExecuteTurn() {

  std::list<Projectile*>::iterator it1 = projectiles.begin();
  while (it1 != projectiles.end()) {
    Projectile *projectile = *it1++;
    if (projectile->IsAlive()) {
      projectile->PerformAction();
    } else {
      RemoveProjectile(projectile);
    }
  }

  std::list<std::shared_ptr<GameUnit>>::iterator it2 = units.begin();
  while (it2 != units.end()) {
    std::shared_ptr<GameUnit> unit = *it2++;
    if (unit->IsAlive()) {
      UpdateSightMap(*unit);
      unit->PerformAction();
    } else {
      RemoveUnit(unit);
    }
  }
}

void GameState::UpdateSightMap(GameUnit& unit) {
  // TODO: Vision should not depend on attack ability.
  unit.SightMap().Create(static_cast<AttackAbility *>(unit.GetAbility("Attack"))->AttackPoint(), GetWalls());
}


void GameState::AddUnit(int type,
                        PlayerNumber owner,
                        const Vector2f &location,
                        float rotation) {
	const UnitAttributes &attributes = unitDefinitions[type];
	auto unit = std::make_shared<GameUnit>(++lastUnitId, attributes, owner, location, rotation);

  // TODO: remove the order dependency.
  UnitAbility *move = new MoveAbility(unit, *this);
  unit->AddAbility(move);

  UnitAbility *attack = new AttackAbility(
    unit,
    this,
    attributes.AttackDamage(),
    attributes.Dispersion(),
    attributes.AttackSpeed(),
    attributes.ProjectileSpeed(),
    attributes.AttackRange());

  unit->AddAbility(attack);

  UnitAbility *autoAttack = new AutoAttackAbility(*unit, *this);
  unit->AddAbility(autoAttack);
  unit->SetIdleAbility(autoAttack);

  UnitAbility *attackMove = new AttackMoveAbility(*unit, *this);
  unit->AddAbility(attackMove);

  unit->AddAbility(new TargetGroundAbility(*unit, *this));

  unit->AddPassiveAbility(new HealthRegenAbility(*unit, attributes.HealthRegen()));

	units.push_back(unit);
	AddToUnitGrid(unit);
	//AddToPathingGrid(unit);
	if (unit->Attributes().CollisionRadius() > maxUnitRadius) {
		maxUnitRadius = unit->Attributes().CollisionRadius();
	}
	unitTable[unit->Id()] = unit;
}

void GameState::RemoveUnit(std::shared_ptr<GameUnit> unit) {
	units.remove(unit);
	unitTable.erase(unit->Id());
	RemoveFromUnitGrid(unit);
	//RemoveFromPathingGrid(unit);
}

std::shared_ptr<GameUnit> GameState::GetUnit(UnitId id) const {
	std::shared_ptr<GameUnit> unit = NULL;
	if (unitTable.count(id)) unit = unitTable.at(id);
	return unit;
}

void GameState::AddProjectile(std::shared_ptr<GameUnit> owner,
                              const Vector2f &location,
                              float direction,
                              float damage,
                              float dispersion,
                              float speed) {
	Projectile *projectile = new Projectile(*this, owner, location, direction,
      damage, dispersion, speed);
	projectiles.push_back(projectile);
}

void GameState::RemoveProjectile(Projectile *projectile) {
  projectiles.remove(projectile);
}

void GameState::AddToUnitGrid(std::shared_ptr<GameUnit> unit) {
	int x = (int) (unit->Position().x / kUnitGridResolution);
	int y = (int) (unit->Position().y / kUnitGridResolution);
	unitGrid[x][y].push_back(unit);
}

void GameState::RemoveFromUnitGrid(const std::shared_ptr<GameUnit> unit) {
  RemoveFromUnitGrid(unit, unit->Position());
}

void GameState::RemoveFromUnitGrid(const std::shared_ptr<GameUnit> unit, const Vector2f &position) {
	int x = int(position.x / kUnitGridResolution);
	int y = int(position.y / kUnitGridResolution);

  unitGrid[x][y].remove(unit);
}

void GameState::UpdateUnitGrid(std::shared_ptr<GameUnit> unit,
                               const Vector2f &previousPosition) {
	int prevX = int(previousPosition.x / kUnitGridResolution);
	int prevY = int(previousPosition.y / kUnitGridResolution);
	int x = (int) (unit->Position().x / kUnitGridResolution);
	int y = (int) (unit->Position().y / kUnitGridResolution);
	if (x != prevX || y != prevY) {
		RemoveFromUnitGrid(unit, previousPosition);
		AddToUnitGrid(unit);
	}
}

std::vector<std::shared_ptr<GameUnit> > GameState::GetUnitsInRectangle(const Vector2f &corner1,
		const Vector2f &corner2) const {
	std::vector<std::shared_ptr<GameUnit> > units;
	int margin = int(maxUnitRadius / kUnitGridResolution + 1);
	float left = std::min(corner1.x, corner2.x);
	float right = std::max(corner1.x, corner2.x);
	float top = std::min(corner1.y, corner2.y);
	float bottom = std::max(corner1.y, corner2.y);
	int start_x = int(left / kUnitGridResolution - margin);
	int start_y = int(top / kUnitGridResolution - margin);
	int end_x = int(right / kUnitGridResolution + margin);
	int end_y = int(bottom / kUnitGridResolution + margin);
	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;
	if (end_x >= unitGridWidth) end_x = unitGridWidth - 1;
	if (end_y >= unitGridHeight) end_y = unitGridHeight - 1;
	for (int i = start_x; i <= end_x; ++i) {
		for (int j = start_y; j <= end_y; ++j) {
			for (std::list<std::shared_ptr<GameUnit>>::const_iterator unit =
					unitGrid[i][j].begin();
					unit != unitGrid[i][j].end();
					++unit) {
				float r = (*unit)->Attributes().SelectionRadius();
				float x = (*unit)->Position().x;
				float y = (*unit)->Position().y;
				if (x + r >= left && x - r <= right &&
						y + r >= top && y - r <= bottom) {
					units.push_back(*unit);
				}
			}
		}
	}
	return units;
}

std::vector<std::shared_ptr<GameUnit>> GameState::GetUnitsInCircle(const Vector2f &center,
		float radius) const {
	std::vector<std::shared_ptr<GameUnit>> units;
	float effectiveRadius = radius + maxUnitRadius;

  Vector2i topLeft;
  topLeft.x = int((center.x - effectiveRadius) / kUnitGridResolution);
  topLeft.y = int((center.y - effectiveRadius) / kUnitGridResolution);
  topLeft = Util::Constrain2(topLeft, Vector2i(0, 0),
    Vector2i(unitGridWidth, unitGridHeight));

  Vector2i bottomRight;
  bottomRight.x = int((center.x + effectiveRadius) / kUnitGridResolution);
  bottomRight.y = int((center.y + effectiveRadius) / kUnitGridResolution);
  bottomRight = Util::Constrain2(bottomRight, Vector2i(0, 0),
      Vector2i(unitGridWidth - 1, unitGridHeight - 1));

  for (int i = topLeft.x; i <= bottomRight.x; ++i) {
    for (int j = topLeft.y; j <= bottomRight.y; ++j) {
      for (std::shared_ptr<GameUnit> unit : unitGrid[i][j]) {
        float distance = Util::Distance(unit->Position(), center);
        if (distance <= effectiveRadius) {
          units.push_back(unit);
        }
      }
		}
	}
	return units;
}

const std::vector<const Poly> &GameState::GetWalls() const {
  return pathingGraph->Walls();
}

std::vector<std::shared_ptr<GameUnit>> GameState::GetUnitsInRectangle(
  const Rect& rectangle) const {
    return GetUnitsInRectangle(rectangle.topLeft, rectangle.bottomRight);
}

CollisionTestResult GameState::TestUnitCollision(const Vector2f& start,
    const Vector2f& end, float radius,
    std::shared_ptr<const GameUnit> unitToIgnore) const {
  DirectedSegment movement(start, end);
  float movementDist = Util::Length(start - end);
  std::shared_ptr<GameUnit> unitHit = nullptr;

  if (movementDist != 0) {
    Rect boundingRectangle = Util::BoundingRectangle(movement);
    boundingRectangle.Grow(radius);

    std::vector<std::shared_ptr<GameUnit>> nearbyUnits =
        GetUnitsInRectangle(boundingRectangle);

    for (std::shared_ptr<GameUnit> otherUnit : nearbyUnits) {
      if (otherUnit != unitToIgnore) {
        float totalRadius = radius
            + otherUnit->Attributes().CollisionRadius();
        Vector2f *collisionPoint =
          movement.CollisionPoint(otherUnit->Position(), totalRadius);

        if (collisionPoint) {
          float distance = Util::Distance(*collisionPoint, start);
          movement.Resize(distance);
          unitHit = otherUnit;
          delete collisionPoint;
        }
      }
    }
  }

  return CollisionTestResult(movement.End(), unitHit);  
}

//CollisionTestResult GameState::TestUnitCollision(const GameUnit& unit,
//                                                 const Vector2f& end,
//                                                 float radius) const {
//  DirectedSegment movement(unit.Position(), end);
//  float movementDist = Util::Length(unit.Position() - end);
//  std::shared_ptr<GameUnit> unitHit = nullptr;
//
//  if (movementDist != 0) {
//    Rect boundingRectangle = Util::BoundingRectangle(movement);
//    boundingRectangle.Grow(radius);
//
//    std::vector<std::shared_ptr<GameUnit>> nearbyUnits =
//        GetUnitsInRectangle(boundingRectangle);
//
//    for (std::shared_ptr<GameUnit> otherUnit : nearbyUnits) {
//      if (*otherUnit != unit) {
//        float totalRadius = radius
//            + otherUnit->Attributes().CollisionRadius();
//        Vector2f *collisionPoint =
//          movement.CollisionPoint(otherUnit->Position(), totalRadius);
//
//        if (collisionPoint) {
//          float distance = Util::Distance(*collisionPoint, unit.Position());
//          movement.Resize(distance);
//          unitHit = otherUnit;
//          delete collisionPoint;
//        }
//      }
//    }
//  }
//
//  return CollisionTestResult(movement.End(), unitHit);
//}

// TODO: Fix or remove this method.
CollisionTestResult GameState::TestWallCollision(const Vector2f& start,
                                                 const Vector2f& end,
                                                 float radius) const {
  DirectedSegment movement(start, end);
  float movementDist = Util::Length(start - end);
  if (movementDist != 0) {
    Rect boundingRect = Util::BoundingRectangle(movement);
    boundingRect.topLeft -= Vector2f(radius, radius);
    boundingRect.bottomRight += Vector2f(radius, radius);
    std::vector<Rect> walls;
    //std::vector<Rect> walls = GetWallsInRectangle(boundingRect);
    std::vector<Rect>::const_iterator it = walls.begin();
    while (it != walls.end()) {
      Rect wall = *it++;
      Vector2f *collisionPoint = movement.CollisionPoint(wall, radius);
      if (collisionPoint) {
        movement = DirectedSegment(start, *collisionPoint);
      }
    }
  }

  return CollisionTestResult(movement.End(), nullptr);  
}

//CollisionTestResult GameState::TestWallCollision(const GameUnit& unit,
//                                                 const Vector2f& end) const {
//  return TestWallCollision(unit.Position(), end,
//      unit.Attributes().CollisionRadius());
//}

CollisionTestResult GameState::TestCollision(const Vector2f& start,
    const Vector2f& end, float radius,
    std::shared_ptr<const GameUnit> unitToIgnore) const {

  CollisionTestResult unitCollision
      = TestUnitCollision(start, end, radius, unitToIgnore);
  CollisionTestResult wallCollision = TestWallCollision(start, end, radius);
  float distToUnit = Util::Distance(unitCollision.point, start);
  float distToWall = Util::Distance(wallCollision.point, start);
  if (distToUnit < distToWall) {
    return unitCollision;
  } else {
    return wallCollision;
  }
}

CollisionTestResult GameState::TestCollision(
    std::shared_ptr<const GameUnit> unit, const Vector2f& end) const {

  return TestCollision(unit->Position(), end,
      unit->Attributes().CollisionRadius(), unit);
}

size_t GameState::HashCode() const {
  size_t hash = Util::HashStart();
  for (auto unit : units) {
    Util::Hash(hash, unit->HashCode());
  }
  for (auto projectile : projectiles) {
    Util::Hash(hash, projectile->HashCode());
  }
  return hash;
}

const float GameScene::kUnitGridResolution = 25.f;

GameScene::GameScene(const GameState &gameState) : walls(gameState.PathingGraph().DilatedWalls()) {
  mapSize = gameState.map_size();
	unit_grid_width_ = int(gameState.map_size().x / kUnitGridResolution);
	unit_grid_height_ = int(gameState.map_size().y / kUnitGridResolution);
	unit_grid_ = new std::list<const UnitModel *> *[unit_grid_width_];
	for (int i = 0; i < unit_grid_width_; ++i) {
		unit_grid_[i] = new std::list<const UnitModel *>[unit_grid_height_];
	}

	max_unit_radius_ = 0;

	for (std::shared_ptr<GameUnit> unit : gameState.Units()) {
		CreateUnit(*unit);
	}

	for (Projectile*projectile : gameState.Projectiles()) {
		projectiles_.push_back(new ProjectileModel(*projectile));
	}
}

// TODO: remove duplication.
GameScene::GameScene(const GameScene &scene1,
                     const GameScene &scene2,
                     float weight) : walls(scene1.walls) {
  assert(weight >= 0.f);
  assert(weight <= 1.f);

	unit_grid_width_ = scene1.unit_grid_width_;
	unit_grid_height_ = scene2.unit_grid_height_;
	unit_grid_ = new std::list<const UnitModel*> *[unit_grid_width_];
	for (int i = 0; i < unit_grid_width_; ++i) {
		unit_grid_[i] = new std::list<const UnitModel*>[unit_grid_height_];
	}
	max_unit_radius_ = 0;
	std::list<UnitModel*>::const_iterator unit_iterator1 =
			scene1.units().begin();
	std::list<UnitModel*>::const_iterator unit_iterator2 =
			scene2.units().begin();
	while (unit_iterator1 != scene1.units().end()) {
		UnitModel &unit1 = **unit_iterator1;
		if (unit_iterator2 != scene2.units().end()) {
			UnitModel &unit2 = **unit_iterator2;
			if (unit1.Id() == unit2.Id()) {
				CreateUnit(unit1, unit2, weight);
				++unit_iterator1;
				++unit_iterator2;
			} else if (unit1.Id() < unit2.Id()) {
				CreateUnit(unit1);
				++unit_iterator1;
			} else {
				++unit_iterator2;
			}
		} else {
			units_.push_back(new UnitModel(unit1));
			++unit_iterator1;
		}
	}
	std::list<ProjectileModel *>::const_iterator projectile_iterator1,
			projectile_iterator2;
	projectile_iterator1 = scene1.projectiles().begin();
	projectile_iterator2 = scene2.projectiles().begin();
	while (projectile_iterator1 != scene1.projectiles().end()) {
		ProjectileModel projectile1 = **projectile_iterator1;
		if (projectile_iterator2 != scene2.projectiles().end()) {
			ProjectileModel projectile2 = **projectile_iterator2;
			if (projectile1.Id() == projectile2.Id()) {
				projectiles_.push_back(new ProjectileModel(projectile1,
						projectile2, weight));
				++projectile_iterator1;
				++projectile_iterator2;
			} else if (projectile1.Id() < projectile2.Id()) {
				projectiles_.push_back(new ProjectileModel(projectile1));
				++projectile_iterator1;
			} else {
				++projectile_iterator2;
			}
		} else {
			projectiles_.push_back(new ProjectileModel(projectile1));
			++projectile_iterator1;
		}
	}

  unitViews = scene2.unitViews;
}

GameScene::~GameScene() {
	for (std::list<UnitModel *>::iterator unit = units_.begin();
			unit != units_.end(); ++unit) {
		delete *unit;
	}
	for (std::list<ProjectileModel *>::iterator projectile =
			projectiles_.begin();
			projectile != projectiles_.end(); ++projectile) {
		delete *projectile;
	}
	for (int i = 0; i < unit_grid_width_; ++i) {
		delete[] unit_grid_[i];
	}
	delete[] unit_grid_;
}

void GameScene::AddUnit(UnitModel *model) {
	units_.push_back(model);
	AddToUnitGrid(*model);
	if (model->Radius() > max_unit_radius_) max_unit_radius_ = model->Radius();
	unit_table_[model->Id()] = model;
}

void GameScene::CreateUnit(const GameUnit &unit) {
	UnitModel *model = new UnitModel(unit);
	AddUnit(model);
}

void GameScene::CreateUnit(const UnitModel &unit1,
                           const UnitModel &unit2,
                           float weight) {
	UnitModel *model = new UnitModel(unit1, unit2, weight);
	AddUnit(model);
}

void GameScene::CreateUnit(const UnitModel &unit) {
	UnitModel *model = new UnitModel(unit);
	AddUnit(model);
}

void GameScene::AddToUnitGrid(const UnitModel &unit) {
	int x = (int) (unit.Position().x / kUnitGridResolution);
	int y = (int) (unit.Position().y / kUnitGridResolution);
	unit_grid_[x][y].push_back(&unit);
}

std::vector<const UnitModel *> GameScene::GetUnitsInRectangle(
		const Vector2f &corner1,
		const Vector2f &corner2) const {
	std::vector<const UnitModel *> units;
	int margin = int(max_unit_radius_ / kUnitGridResolution + 1);
	float left = std::min(corner1.x, corner2.x);
	float right = std::max(corner1.x, corner2.x);
	float top = std::min(corner1.y, corner2.y);
	float bottom = std::max(corner1.y, corner2.y);
	int start_x = int(left / kUnitGridResolution - margin);
	int start_y = int(top / kUnitGridResolution - margin);
	int end_x = int(right / kUnitGridResolution + margin);
	int end_y = int(bottom / kUnitGridResolution + margin);
	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;
	if (end_x >= unit_grid_width_) end_x = unit_grid_width_ - 1;
	if (end_y >= unit_grid_height_) end_y = unit_grid_height_ - 1;
	for (int i = start_x; i <= end_x; ++i) {
		for (int j = start_y; j <= end_y; ++j) {
			for (const UnitModel* unit : unit_grid_[i][j]) {
				float r = unit->Radius();
				float x = unit->Position().x;
				float y = unit->Position().y;
				if (x + r >= left && x - r <= right &&
						y + r >= top && y - r <= bottom) {
					units.push_back(unit);
				}
			}
		}
	}
	return units;
}

UnitModel *GameScene::GetUnit(UnitId id) const {
	UnitModel *unit = NULL;
	if (unit_table_.count(id)) unit = unit_table_.at(id);
	return unit;
}