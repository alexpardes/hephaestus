#include "stdafx.h"
#include "GameState.h"
#include "Util.h"
#include "MoveAbility.h"
#include "AttackAbility.h"
#include "AttackMoveAbility.h"
#include "SectorMap.h"

const float GameState::kPathingResolution = 25.f;
const float GameState::kUnitGridResolution = 25.f;

GameState::GameState(const UnitDictionary &unit_dictionary, 
                     const Vector2i &map_size,
                     PathFinder *pathfinder) : 
    unitDefinitions(unit_dictionary) {
  this->pathfinder = pathfinder;
  pathfinder->SetTileSize(Vector2f(kTileSize, kTileSize));
	max_unit_radius_ = 0.f;
	map_size_ = map_size;
	Vector2f map_pixel_size = kTileSize * Util::GetVector2f(map_size);
	pathing_width_ = (int) std::ceilf(map_pixel_size.x / kPathingResolution);
	pathing_height_ = (int) std::ceilf(map_pixel_size.y / kPathingResolution);
	pathing_grid_ = new int*[pathing_width_];
	for (int i = 0; i < pathing_width_; ++i) {
		pathing_grid_[i] = new int[pathing_height_];
		for (int j = 0; j < pathing_height_; ++j) {
			pathing_grid_[i][j] = 0;
		}
	}
	unit_grid_width_ = (int) std::ceilf(map_pixel_size.x / kUnitGridResolution);
	unit_grid_height_ = (int) std::ceilf(map_pixel_size.y / kUnitGridResolution);
  unit_grid_ = UnitGrid(unit_grid_width_);  
	for (int i = 0; i < unit_grid_width_; ++i) {
    unit_grid_[i] = std::vector<std::list<std::shared_ptr<GameUnit>>>(unit_grid_height_);
	}

  lastUnitId = 0;
}

GameState::~GameState() {
	for (int i = 0; i < pathing_width_; ++i) {
		delete[] pathing_grid_[i];
	}

	delete[] pathing_grid_;

	for (std::list<Projectile *>::iterator projectile = projectiles_.begin();
			projectile != projectiles_.end(); ++projectile) {
		delete *projectile;
	}
}

Vector2f GameState::GetUnitPosition(UnitId id) const {
  return GetUnit(id)->Position();
}

void GameState::MoveUnit(UnitId id, Vector2f location) {
  std::shared_ptr<GameUnit> unit = GetUnit(id);
  Vector2f previousPosition = unit->Position();
  unit->SetPosition(Constrain(location));
  UpdateUnitGrid(unit, previousPosition);
}

Vector2f GameState::Constrain(Vector2f location) const {
  if (location.x < 0) {
    location.x = 0.f;
  } else if (location.x > kTileSize * map_size().x) {
    location.x = kTileSize * map_size().x;
  }
  if (location.y < 0) {
    location.y = 0.f;
  } else if (location.y > kTileSize * map_size().y) {
    location.y = kTileSize * map_size().y;
  }
  return location;
}

Vector2i GameState::Constrain(Vector2i location) const {
  if (location.x < 0) {
    location.x = 0.f;
  } else if (location.x >= map_size().x) {
    location.x = map_size().x - 1;
  }
  if (location.y < 0) {
    location.y = 0.f;
  } else if (location.y >= map_size().y) {
    location.y = map_size().y - 1;
  }
  return location;
}

//TODO: remove duplication; don't modify lists during iteration.
void GameState::ExecuteTurn() {

  std::list<Projectile*>::iterator it1 = projectiles_.begin();
  while (it1 != projectiles_.end()) {
    Projectile *projectile = *it1++;
    if (projectile->IsAlive()) {
      projectile->PerformAction();
    } else {
      RemoveProjectile(projectile);
    }
  }

  std::list<std::shared_ptr<GameUnit>>::iterator it2 = units_.begin();
  while (it2 != units_.end()) {
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
  unit.SightMap().Clear();
  unit.SightMap().SetCenter(unit.Position());
  std::vector<Rect> walls = GetWalls();

  for (Rect wall : walls) {
    std::vector<Vector2f> widestPoints = wall.WidestPoints(unit.Position());
    float startAngle = Util::FindAngle(widestPoints[0] - unit.Position());
    float endAngle = Util::FindAngle(widestPoints[1] - unit.Position());
    float distance = Util::Distance((widestPoints[0] + widestPoints[1])/2.f,
      unit.Position());
    unit.SightMap().Add(startAngle, endAngle, distance);
  }  
}


void GameState::AddUnit(const std::string &type,
                        PlayerNumber owner,
                        const Vector2f &location,
                        float rotation) {
	const UnitAttributes &attributes = unitDefinitions.at(type);
	std::shared_ptr<GameUnit> unit(new GameUnit(++lastUnitId, attributes, owner, location, rotation));

  // TODO: remove the order dependency.
  UnitAbility *move = new MoveAbility(unit, pathfinder, this, attributes.speed());
  unit->AddAbility(move);

  UnitAbility *attack = new AttackAbility(unit, this,
      attributes.attack_damage(), attributes.attack_speed(), attributes.attack_range());
  unit->AddAbility(attack);

  UnitAbility *attackMove = new AttackMoveAbility(*unit, *this, 400, 450);
  unit->AddAbility(attackMove);

	units_.push_back(unit);
	AddToUnitGrid(unit);
	AddToPathingGrid(unit);
	if (unit->Attributes().CollisionRadius() > max_unit_radius_) {
		max_unit_radius_ = unit->Attributes().CollisionRadius();
	}
	unit_table_[unit->Id()] = unit;
}

void GameState::RemoveUnit(std::shared_ptr<GameUnit> unit) {
	units_.remove(unit);
	unit_table_.erase(unit->Id());
	RemoveFromUnitGrid(unit);
	RemoveFromPathingGrid(unit);
}

std::shared_ptr<GameUnit> GameState::GetUnit(UnitId id) const {
	std::shared_ptr<GameUnit> unit = NULL;
	if (unit_table_.count(id)) unit = unit_table_.at(id);
	return unit;
}

void GameState::AddProjectile(const std::string &name,
                              const Vector2f &location,
                              std::shared_ptr<GameUnit> target,
                              float damage,
                              float speed) {
	Projectile *projectile = new Projectile(name, location, target,
      damage, speed);
	projectiles_.push_back(projectile);
}

void GameState::RemoveProjectile(Projectile *projectile) {
  projectiles_.remove(projectile);
}

void GameState::AddToUnitGrid(std::shared_ptr<GameUnit> unit) {
	int x = (int) (unit->Position().x / kUnitGridResolution);
	int y = (int) (unit->Position().y / kUnitGridResolution);
	unit_grid_[x][y].push_back(unit);
}

void GameState::RemoveFromUnitGrid(const std::shared_ptr<GameUnit> unit) {
  RemoveFromUnitGrid(unit, unit->Position());
}

void GameState::RemoveFromUnitGrid(const std::shared_ptr<GameUnit> unit, const Vector2f &position) {
	int x = int(position.x / kUnitGridResolution);
	int y = int(position.y / kUnitGridResolution);

  unit_grid_[x][y].remove(unit);
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
			for (std::list<std::shared_ptr<GameUnit>>::const_iterator unit =
					unit_grid_[i][j].begin();
					unit != unit_grid_[i][j].end();
					++unit) {
				float r = (*unit)->Attributes().selection_radius();
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
	float effectiveRadius = radius + max_unit_radius_;

  Vector2i topLeft;
  topLeft.x = int((center.x - effectiveRadius) / kUnitGridResolution);
  topLeft.y = int((center.y - effectiveRadius) / kUnitGridResolution);
  topLeft = Util::Constrain2(topLeft, Vector2i(0, 0),
    Vector2i(unit_grid_width_, unit_grid_height_));

  Vector2i bottomRight;
  bottomRight.x = int((center.x + effectiveRadius) / kUnitGridResolution);
  bottomRight.y = int((center.y + effectiveRadius) / kUnitGridResolution);
  bottomRight = Util::Constrain2(bottomRight, Vector2i(0, 0),
      Vector2i(unit_grid_width_ - 1, unit_grid_height_ - 1));

  for (int i = topLeft.x; i <= bottomRight.x; ++i) {
    for (int j = topLeft.y; j <= bottomRight.y; ++j) {
      for (std::shared_ptr<GameUnit> unit : unit_grid_[i][j]) {
        float distance = Util::Distance(unit->Position(), center);
        if (distance <= effectiveRadius) {
          units.push_back(unit);
        }
      }
		}
	}
	return units;
}

Rect GameState::GetWall(const Vector2i& tile) const {
  Rect wall;
  wall.topLeft = Vector2f(tile.x*kTileSize, tile.y*kTileSize);
  wall.bottomRight = Vector2f((tile.x + 1)*kTileSize, (tile.y + 1)*kTileSize);
  return wall;
}

std::vector<Rect> GameState::GetWalls() const {
  std::vector<Rect> walls;
  for (int x = 0; x < map_size_.x; ++x) {
    for (int y = 0; y < map_size_.y; ++y) {
      if (pathfinder->GetPathingGrid()->IsBlocked(x, y)) {
        walls.push_back(GetWall(Vector2i(x, y)));
      }
    }
  }
  return walls;
}

std::vector<Rect>
GameState::GetWallsInRectangle(const Rect &rectangle) const {
  Vector2i topLeft = GetTile(rectangle.topLeft);
  Vector2i bottomRight = GetTile(rectangle.bottomRight);

  std::vector<Rect> walls;
  for (int x = topLeft.x; x <= bottomRight.x; ++x) {
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
      if (pathfinder->GetPathingGrid()->IsBlocked(x, y)) {
        walls.push_back(GetWall(Vector2i(x, y)));
      }
    }
  }
  return walls;
}

std::vector<std::shared_ptr<GameUnit>> GameState::GetUnitsInRectangle(
  const Rect& rectangle) const {
    return GetUnitsInRectangle(rectangle.topLeft, rectangle.bottomRight);
}

void GameState::AddToPathingGrid(const std::shared_ptr<GameUnit> unit) {
	AdjustPathingGrid(unit, 1);
}

void GameState::RemoveFromPathingGrid(const std::shared_ptr<GameUnit> unit) {
	AdjustPathingGrid(unit, -1);
}

void GameState::AdjustPathingGrid(const std::shared_ptr<GameUnit> unit, int value) {
	int center_x = (int) (unit->Position().x / kPathingResolution);
	int center_y = (int) (unit->Position().y / kPathingResolution);
	int start_x = (int) ((unit->Position().x -
			unit->Attributes().CollisionRadius()) /	kPathingResolution);
	int end_x =	(int) ((unit->Position().x +
			unit->Attributes().CollisionRadius()) / kPathingResolution);
	int start_y = (int) ((unit->Position().y -
			unit->Attributes().CollisionRadius()) / kPathingResolution);
	int end_y =	(int) ((unit->Position().y +
			unit->Attributes().CollisionRadius()) / kPathingResolution);
	float radius2 = unit->Attributes().CollisionRadius() *
			unit->Attributes().CollisionRadius();
	for (int i = start_x; i <= end_x; ++i) {
		if (i == center_x) {
			for (int j = start_y; j <= end_y; ++j) {
				pathing_grid_[i][j] += value;
			}
		} else {
			float x_coord = kPathingResolution * i;
			if (i < center_x) x_coord += kPathingResolution;
			for (int j = start_y; j <= end_y; ++j) {
				if (j == center_y) {
					pathing_grid_[i][j] += value;
				} else {
					float y_coord = kPathingResolution * j;
					if (j < center_y) y_coord += kPathingResolution;
					if (Util::Distance2(Vector2f(x_coord, y_coord),
										unit->Position()) <= radius2) {
						pathing_grid_[i][j] += value;
					}
				}
			}
		}
	}
}

void GameState::AddTerrain(const Vector2f &top_left,
						   const Vector2f &bottom_right) {
	int top = int(top_left.y / kPathingResolution);
	if (top < 0) top = 0;
	int left = int(top_left.x / kPathingResolution);
	if (left < 0) left = 0;
	int bottom = int(bottom_right.y / kPathingResolution);
	if (bottom >= pathing_height_) bottom = pathing_height_ - 1;
	int right = int(bottom_right.x / kPathingResolution);
	if (right >= pathing_width_) right = pathing_width_ - 1;
	for (int x = left; x <= right; ++x) {
		for (int y = top; y <= bottom; ++y) {
			++pathing_grid_[x][y];
		}
	}
}

Vector2i GameState::GetTile(UnitId id) const {
  return GetTile(GetUnit(id)->Position());
}

Vector2i GameState::GetTile(const Vector2f &gameLocation) const {
  return Vector2i(gameLocation.x / kTileSize, gameLocation.y / kTileSize);
}

// Returns the centerpoint of the input tile.
Vector2f GameState::GetLocation(const Vector2i &gridLocation) const {
  return Vector2f((gridLocation.x + 0.5) * kTileSize,
      (gridLocation.y + 0.5) * kTileSize);
}

float GameState::DistanceToUnit(const GameUnit& unit,
                                const Vector2f& end,
                                float radius) const {
  DirectedSegment movement(unit.Position(), end);
  float movementDist = Util::Length(unit.Position() - end);

  if (movementDist != 0) {
    Rect boundingRectangle = Util::BoundingRectangle(movement);
    boundingRectangle.Grow(radius);

    std::vector<std::shared_ptr<GameUnit>> nearbyUnits =
        GetUnitsInRectangle(boundingRectangle);

    for (std::shared_ptr<GameUnit> otherUnit : nearbyUnits) {
      if (*otherUnit != unit) {
        float totalRadius = radius
            + otherUnit->Attributes().CollisionRadius();
        Vector2f *collisionPoint =
          movement.CollisionPoint(otherUnit->Position(), totalRadius);

        if (collisionPoint) {
          float distance = Util::Distance(*collisionPoint, unit.Position());
          movement.Resize(distance);
          delete collisionPoint;
        }
      }
    }
  }

  return movement.Length();  
}

float GameState::DistanceToWall(const GameUnit& unit,
                                const Vector2f& end,
                                float radius) const {
  DirectedSegment movement(unit.Position(), end);
  float movementDist = Util::Length(unit.Position() - end);
  if (movementDist != 0) {
    Rect boundingRect = Util::BoundingRectangle(movement);
    float radius = unit.Attributes().CollisionRadius();
    boundingRect.topLeft -= Vector2f(radius, radius);
    boundingRect.bottomRight += Vector2f(radius, radius);
    std::vector<Rect> walls = GetWallsInRectangle(boundingRect);
    std::vector<Rect>::const_iterator it = walls.begin();
    while (it != walls.end()) {
      Rect wall = *it++;
      Vector2f *collisionPoint = movement.CollisionPoint(wall, radius);
      if (collisionPoint) {
        movement = DirectedSegment(unit.Position(), *collisionPoint);
      }
    }
  }

  return movement.Length();  
}

float GameState::DistanceToObstacle(const GameUnit& unit,
                                     const Vector2f& end) const {
  return DistanceToObstacle(unit, end, unit.Attributes().CollisionRadius());
}

float GameState::DistanceToObstacle(const GameUnit& unit,
                                    const Vector2f& end,
                                    float radius) const {
  float distanceToUnit = DistanceToUnit(unit, end, radius);
  float distanceToWall = DistanceToWall(unit, end, radius);
  return std::min(distanceToUnit, distanceToWall);
}

std::vector<sf::ConvexShape> GameState::FindOccludedAreas(
    const GameUnit& unit) const {
  std::vector<sf::ConvexShape> result;
  std::vector<Rect> walls = GetWalls();

  for (Rect wall : walls) {
    std::vector<Vector2f> widestPoints = wall.WidestPoints(unit.Position());
    Vector2f direction1 = widestPoints[0] - unit.Position();
    Util::Resize(direction1, 1000);
    Vector2f farPoint1 = unit.Position() + direction1;

    Vector2f direction2 = widestPoints[1] - unit.Position();
    Util::Resize(direction2, 1000);
    Vector2f farPoint2 = unit.Position() + direction2;

    sf::ConvexShape area(4);
    area.setPoint(0, widestPoints[0]);
    area.setPoint(1, widestPoints[1]);
    area.setPoint(2, farPoint1);
    area.setPoint(3, farPoint2);
    result.push_back(area);
  }

  return result;
}

const float GameScene::kUnitGridResolution = 25.f;

GameScene::GameScene(GameState &game_state) {
  mapSize = game_state.map_size();
	unit_grid_width_ = int(game_state.map_size().x * kTileSize /
			kUnitGridResolution);
	unit_grid_height_ = int(game_state.map_size().y * kTileSize /
			kUnitGridResolution);
	unit_grid_ = new std::list<const UnitModel *> *[unit_grid_width_];
	for (int i = 0; i < unit_grid_width_; ++i) {
		unit_grid_[i] = new std::list<const UnitModel *>[unit_grid_height_];
	}

	max_unit_radius_ = 0;

	for (std::shared_ptr<GameUnit> unit : game_state.units()) {
		CreateUnit(*unit);
	}

	for (Projectile*projectile : game_state.projectiles()) {
		projectiles_.push_back(new ProjectileModel(*projectile));
	}
}

void GameScene::ComputeVisibility(PlayerNumber playerID) {
  std::vector<const SectorMap*> unitViews;
  for (UnitModel* unit : units()) {
    if (unit->Owner() == playerID) {
      unitViews.push_back(unit->SightMap());
    }
  }

  for (int x = 0; x < mapSize.x; ++x) {
    isVisible.push_back(std::vector<bool>());
    for (int y = 0; y < mapSize.y; ++y) {
      bool isTileVisible = false;
      Vector2f tileCenter(kTileSize*(x + 0.5), kTileSize*(y + 0.5));
      for (const SectorMap* view : unitViews) {
        if (view->Contains(tileCenter)) {
          isTileVisible = true;
          break;
        }
      }

      isVisible.back().push_back(isTileVisible);
    }
  }

}


// TODO: remove duplicated code.
GameScene::GameScene(GameScene &scene1,
					 GameScene &scene2,
					 float weight) {
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

    isVisible = scene2.isVisible;
	}
	std::list<ProjectileModel *>::const_iterator projectile_iterator1,
			projectile_iterator2;
	projectile_iterator1 = scene1.projectiles().begin();
	projectile_iterator2 = scene2.projectiles().begin();
	while (projectile_iterator1 != scene1.projectiles().end()) {
		ProjectileModel projectile1 = **projectile_iterator1;
		if (projectile_iterator2 != scene2.projectiles().end()) {
			ProjectileModel projectile2 = **projectile_iterator2;
			if (projectile1.id() == projectile2.id()) {
				projectiles_.push_back(new ProjectileModel(projectile1,
						projectile2, weight));
				++projectile_iterator1;
				++projectile_iterator2;
			} else if (projectile1.id() < projectile2.id()) {
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
	if (model->radius() > max_unit_radius_) max_unit_radius_ = model->radius();
	unit_table_[model->Id()] = model;
}

void GameScene::CreateUnit(const GameUnit &unit) {
	UnitModel *model = new UnitModel(unit);
	AddUnit(model);
}

void GameScene::CreateUnit(const UnitModel &unit1,
						const UnitModel &unit2,
						float weight) {
	if (weight > 1.f) weight = 1.f;
	UnitModel *model = new UnitModel(unit1, unit2, weight);
	AddUnit(model);
}

void GameScene::CreateUnit(const UnitModel &unit) {
	UnitModel *model = new UnitModel(unit);
	AddUnit(model);
}

void GameScene::AddToUnitGrid(const UnitModel &unit) {
	int x = (int) (unit.position().x / kUnitGridResolution);
	int y = (int) (unit.position().y / kUnitGridResolution);
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
			for (std::list<const UnitModel *>::iterator unit =
					unit_grid_[i][j].begin();
					unit != unit_grid_[i][j].end();
					++unit) {
				float r = (*unit)->radius();
				float x = (*unit)->position().x;
				float y = (*unit)->position().y;
				if (x + r >= left && x - r <= right &&
						y + r >= top && y - r <= bottom) {
					units.push_back(*unit);
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