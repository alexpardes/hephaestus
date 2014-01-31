#include "stdafx.h"
#include "GameState.h"

#include "MoveAbility.h"

const float GameState::kPathingResolution = 25.f;
const float GameState::kUnitGridResolution = 25.f;

GameState::GameState(const UnitDictionary &unit_dictionary, 
                     const Vector2i &map_size,
                     PathFinder *pathfinder) : 
    unit_dictionary_(unit_dictionary) {
  this->pathfinder = pathfinder;
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
	unit_grid_ = new std::list<GameUnit*>*[unit_grid_width_];
	for (int i = 0; i < unit_grid_width_; ++i) {
		unit_grid_[i] = new std::list<GameUnit*>[unit_grid_height_];
	}

  lastUnitId = 0;
}

GameState::~GameState() {
	for (int i = 0; i < pathing_width_; ++i) {
		delete[] pathing_grid_[i];
	}

	delete[] pathing_grid_;

	for (int i = 0; i < unit_grid_width_; ++i) {
		delete[] unit_grid_[i];
	}

	delete[] unit_grid_;

	for (std::list<GameUnit *>::iterator unit = units_.begin();
			unit != units_.end(); ++unit) {
		delete *unit;
	}

	for (std::list<Projectile *>::iterator projectile = projectiles_.begin();
			projectile != projectiles_.end(); ++projectile) {
		delete *projectile;
	}
}

Vector2f GameState::GetUnitPosition(UnitId id) const {
  return GetUnit(id)->position();
}

void GameState::MoveUnit(UnitId id, Vector2f location) {
  GameUnit *unit = GetUnit(id);
  unit->SetPosition(Constrain(location));
  UpdateUnitGrid(*unit);
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

void GameState::ExecuteTurn() {
  std::list<GameUnit*>::iterator it = units_.begin();
  while (it != units_.end()) {
    (**it++).PerformAction();
  }
}


void GameState::AddUnit(const std::string &type,
						PlayerNumber owner,
						const Vector2f &location,
						float rotation) {
	const UnitAttributes &attributes = unit_dictionary_.at(type);
	GameUnit *unit = new GameUnit(++lastUnitId, attributes, owner, location, rotation);
  UnitAbility *ability = new MoveAbility(unit, pathfinder, this, attributes.speed());
  unit->SetAbility(ability);

	units_.push_back(unit);
	AddToUnitGrid(*unit);
	AddToPathingGrid(*unit);
	if (unit->Attributes().collision_radius() > max_unit_radius_) {
		max_unit_radius_ = unit->Attributes().collision_radius();
	}
	unit_table_[unit->Id()] = unit;
}

void GameState::RemoveUnit(GameUnit *unit) {
	units_.remove(unit);
	unit_table_.erase(unit->Id());
	RemoveFromUnitGrid(*unit);
	RemoveFromPathingGrid(*unit);
	delete unit;
}

GameUnit *GameState::GetUnit(UnitId id) const {
	GameUnit *unit = NULL;
	if (unit_table_.count(id)) unit = unit_table_.at(id);
	return unit;
}

void GameState::AddProjectile(const Vector2f &location,
							  GameUnit *target,
							  float damage) {
	Projectile *projectile = new Projectile(location, target, damage);
	projectiles_.push_back(projectile);
}

void GameState::RemoveProjectile(Projectile *projectile) {
	for (std::list<Projectile *>::iterator projectile_iterator =
			projectiles_.begin(); projectile_iterator != projectiles_.end();
			++projectile_iterator) {
		if (*projectile_iterator == projectile) {
			projectiles_.erase(projectile_iterator);
			break;
		}
	}
}

void GameState::AddToUnitGrid(GameUnit &unit) {
	int x = (int) (unit.position().x / kUnitGridResolution);
	int y = (int) (unit.position().y / kUnitGridResolution);
	unit_grid_[x][y].push_back(&unit);
	unit.set_unit_grid_x(x);
	unit.set_unit_grid_y(y);
}

void GameState::RemoveFromUnitGrid(GameUnit &unit) {
	int x = unit.unit_grid_x();
	int y = unit.unit_grid_y();
	for (std::list<GameUnit*>::iterator i =
			unit_grid_[x][y].begin();
			i != unit_grid_[x][y].end();
			++i) {
		if (*i == &unit) {
			unit_grid_[x][y].erase(i);
			break;
		}
	}
}

void GameState::UpdateUnitGrid(GameUnit &unit) {
	int prev_x = unit.unit_grid_x();
	int prev_y = unit.unit_grid_y();
	int x = (int) (unit.position().x / kUnitGridResolution);
	int y = (int) (unit.position().y / kUnitGridResolution);
	if (x != unit.unit_grid_x() || y != unit.unit_grid_y()) {
		RemoveFromUnitGrid(unit);
		AddToUnitGrid(unit);
	}
}

std::vector<GameUnit *> GameState::GetUnitsInRectangle(const Vector2f &corner1,
		const Vector2f &corner2) const {
	std::vector<GameUnit *> units;
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
			for (std::list<GameUnit*>::iterator unit =
					unit_grid_[i][j].begin();
					unit != unit_grid_[i][j].end();
					++unit) {
				float r = (*unit)->Attributes().selection_radius();
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

std::vector<GameUnit *> GameState::GetUnitsInCircle(const Vector2f &center,
		float radius) const {
	std::vector<GameUnit *> units;
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
      Vector2i(unit_grid_width_, unit_grid_height_));

  for (int i = topLeft.x; i <= bottomRight.x; ++i) {
    for (int j = topLeft.y; j <= bottomRight.y; ++j) {
			for (std::list<GameUnit *>::iterator unit =
					unit_grid_[i][j].begin();
					unit != unit_grid_[i][j].end();
					++unit) {
				float distance = Util::Distance((*unit)->position(), center);
				if (distance <= effectiveRadius) {
					units.push_back(*unit);
				}
			}
		}
	}
	return units;
}

std::vector<Rect>
GameState::GetWallsInRectangle(const Rect &rectangle) const {
  Vector2i topLeft = GetTile(rectangle.topLeft);
  Vector2i bottomRight = GetTile(rectangle.bottomRight);

  std::vector<Rect> walls;
  for (int x = topLeft.x; x <= bottomRight.x; ++x) {
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
      if (pathfinder->GetPathingGrid()->IsBlocked(x, y)) {
        Rect wall;
        wall.topLeft = Vector2f(x*kTileSize, y*kTileSize);
        wall.bottomRight = Vector2f((x + 1)*kTileSize, (y + 1)*kTileSize);
        walls.push_back(wall);
      }
    }
  }
  return walls;
}

void GameState::AddToPathingGrid(const GameUnit &unit) {
	AdjustPathingGrid(unit, 1);
}

void GameState::RemoveFromPathingGrid(const GameUnit &unit) {
	AdjustPathingGrid(unit, -1);
}

void GameState::AdjustPathingGrid(const GameUnit &unit, int value) {
	int center_x = (int) (unit.position().x / kPathingResolution);
	int center_y = (int) (unit.position().y / kPathingResolution);
	int start_x = (int) ((unit.position().x -
			unit.Attributes().collision_radius()) /	kPathingResolution);
	int end_x =	(int) ((unit.position().x +
			unit.Attributes().collision_radius()) / kPathingResolution);
	int start_y = (int) ((unit.position().y -
			unit.Attributes().collision_radius()) / kPathingResolution);
	int end_y =	(int) ((unit.position().y +
			unit.Attributes().collision_radius()) / kPathingResolution);
	float radius2 = unit.Attributes().collision_radius() *
			unit.Attributes().collision_radius();
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
										unit.position()) <= radius2) {
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
  return GetTile(GetUnit(id)->position());
}

Vector2i GameState::GetTile(const Vector2f &gameLocation) const {
  return Vector2i(gameLocation.x / kTileSize, gameLocation.y / kTileSize);
}

// Returns the centerpoint of the input tile.
Vector2f GameState::GetLocation(const Vector2i &gridLocation) const {
  return Vector2f((gridLocation.x + 0.5) * kTileSize,
      (gridLocation.y + 0.5) * kTileSize);
}

const float GameScene::kUnitGridResolution = 25.f;

GameScene::GameScene(GameState &game_state) {
	unit_grid_width_ = int(game_state.map_size().x * kTileSize /
			kUnitGridResolution);
	unit_grid_height_ = int(game_state.map_size().y * kTileSize /
			kUnitGridResolution);
	unit_grid_ = new std::list<const UnitModel *> *[unit_grid_width_];
	for (int i = 0; i < unit_grid_width_; ++i) {
		unit_grid_[i] = new std::list<const UnitModel *>[unit_grid_height_];
	}

	max_unit_radius_ = 0;
	for (std::list<GameUnit *>::const_iterator unit_iterator =
			game_state.units().begin();
			unit_iterator != game_state.units().end();
			++unit_iterator) {
		CreateUnit(**unit_iterator);
	}

	for (std::list<Projectile *>::const_iterator projectile =
			game_state.projectiles().begin();
			projectile != game_state.projectiles().end();
			++projectile) {
		projectiles_.push_back(new ProjectileModel(**projectile));
	}
}

GameScene::GameScene(GameScene &scene1,
					 GameScene &scene2,
					 float weight) {
	unit_grid_width_ = scene1.unit_grid_width_;
	unit_grid_height_ = scene2.unit_grid_height_;
	unit_grid_ = new std::list<const UnitModel *> *[unit_grid_width_];
	for (int i = 0; i < unit_grid_width_; ++i) {
		unit_grid_[i] = new std::list<const UnitModel *>[unit_grid_height_];
	}
	max_unit_radius_ = 0;
	std::list<UnitModel *>::const_iterator unit_iterator1 =
			scene1.units().begin();
	std::list<UnitModel *>::const_iterator unit_iterator2 =
			scene2.units().begin();
	while (unit_iterator1 != scene1.units().end()) {
		UnitModel &unit1 = **unit_iterator1;
		if (unit_iterator2 != scene2.units().end()) {
			UnitModel &unit2 = **unit_iterator2;
			if (unit1.id() == unit2.id()) {
				CreateUnit(unit1, unit2, weight);
				++unit_iterator1;
				++unit_iterator2;
			} else if (unit1.id() < unit2.id()) {
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
	unit_table_[model->id()] = model;
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