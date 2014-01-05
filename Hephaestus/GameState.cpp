#include "stdafx.h"
#include "GameState.h"

const float GameState::kPathingResolution = 25.f;
const float GameState::kUnitGridResolution = 25.f;

GameState::GameState(const UnitDictionary &unit_dictionary,
					 const Vector2i &map_size) :
	unit_dictionary_(unit_dictionary) {
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


void GameState::AddUnit(const std::string &type,
						PlayerNumber owner,
						const Vector2f &location,
						float rotation) {
	const UnitAttributes &attributes = unit_dictionary_.at(type);
	GameUnit *unit = new GameUnit(attributes, owner, location, rotation);
	units_.push_back(unit);
	AddToUnitGrid(*unit);
	AddToPathingGrid(*unit);
	if (unit->attributes().collision_radius() > max_unit_radius_) {
		max_unit_radius_ = unit->attributes().collision_radius();
	}
	unit_table_[unit->id()] = unit;
}

void GameState::RemoveUnit(GameUnit *unit) {
	units_.remove(unit);
	unit_table_.erase(unit->id());
	RemoveFromUnitGrid(*unit);
	if (unit->is_stationary()) {
		RemoveFromPathingGrid(*unit);
	}
	delete unit;
}

GameUnit *GameState::GetUnit(unitId id) const {
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

std::list<GameUnit *> GameState::GetUnitsInRectangle(const Vector2f &corner1,
		const Vector2f &corner2) const {
	std::list<GameUnit *> units;
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
				float r = (*unit)->attributes().selection_radius();
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

std::list<GameUnit *> GameState::GetUnitsInCircle(const Vector2f &center,
		float radius) const {
	std::list<GameUnit *> units;
	float effective_radius = radius + max_unit_radius_;
	float effective_radius2 = Util::Square(effective_radius);
	int start_x = int((center.x - effective_radius) / kUnitGridResolution);
	int start_y = int((center.y - effective_radius) / kUnitGridResolution);
	int end_x = int((center.x + effective_radius) / kUnitGridResolution);
	int end_y = int((center.y + effective_radius) / kUnitGridResolution);
	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;
	if (end_x >= unit_grid_width_) end_x = unit_grid_width_ - 1;
	if (end_y >= unit_grid_height_) end_y = unit_grid_height_ - 1;
	for (int i = start_x; i <= end_x; ++i) {
		for (int j = start_y; j <= end_y; ++j) {
			for (std::list<GameUnit *>::iterator unit =
					unit_grid_[i][j].begin();
					unit != unit_grid_[i][j].end();
					++unit) {
				float distance2 = Util::Distance2((*unit)->position(), center);
				if (distance2 <= effective_radius2) {
					units.push_back(*unit);
				}
			}
		}
	}
	return units;
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
			unit.attributes().collision_radius()) /	kPathingResolution);
	int end_x =	(int) ((unit.position().x +
			unit.attributes().collision_radius()) / kPathingResolution);
	int start_y = (int) ((unit.position().y -
			unit.attributes().collision_radius()) / kPathingResolution);
	int end_y =	(int) ((unit.position().y +
			unit.attributes().collision_radius()) / kPathingResolution);
	float radius2 = unit.attributes().collision_radius() *
			unit.attributes().collision_radius();
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

std::list<const UnitModel *> GameScene::GetUnitsInRectangle(
		const Vector2f &corner1,
		const Vector2f &corner2) const {
	std::list<const UnitModel *> units;
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

UnitModel *GameScene::GetUnit(unitId id) const {
	UnitModel *unit = NULL;
	if (unit_table_.count(id)) unit = unit_table_.at(id);
	return unit;
}