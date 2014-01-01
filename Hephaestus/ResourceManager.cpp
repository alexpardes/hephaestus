#include "stdafx.h"
#include "ResourceManager.h"

GameState *ResourceManager::LoadMap(const std::string &filename) {
	std::ifstream map_file(filename);
	if (!map_file.good()) {
		assert(0);
		return NULL;
	}
	Json::Reader reader;
	Json::Value map;
	reader.parse(map_file, map);
	map_file.close();

	Json::Value tiles = map["tiles"];
	LoadTiles(tiles);

	std::string unit_file = map["unitfile"].asString();
	LoadUnitFile(unit_file);

	int map_width = map["width"].asInt();
	int map_height = map["height"].asInt();
	Json::Value terrain = map["terrain"];
	std::string default_tile_type = map["defaulttile"].asString();
	LoadTerrain(Vector2i(map_width, map_height), terrain,  default_tile_type);
	GameState *state = new GameState(unit_dictionary_, Vector2i(map_width, map_height));
	Json::Value units = map["units"];
	for (Json::Value::iterator unit = units.begin(); unit != units.end();
			++unit) {
		std::string unit_type = (*unit)["type"].asString();
		int owner = (*unit)["owner"].asInt() - 1;
		float x = float((*unit)["x"].asDouble());
		float y = float((*unit)["y"].asDouble());
		float rotation = float((*unit)["rotation"].asDouble());
		state->AddUnit(unit_type, PlayerNumber(owner), Vector2f(x, y), rotation);
	}
	for (int x = 0; x < map_width; ++x) {
		for (int y = 0; y < map_height; ++y) {
			if (!traversability_[terrain_[x][y]]) {
				Vector2f top_left(x*kTileSize, y*kTileSize);
				Vector2f bottom_right = top_left + Vector2f(kTileSize, kTileSize);
				state->AddTerrain(top_left, bottom_right);
			}
		}
	}

	return state;
}

void ResourceManager::LoadUnitFile(const std::string &filename) {
	std::ifstream unit_file(filename + ".udf");
	if (!unit_file.good()) {
		assert(0);
		return;
	}
	Json::Reader reader;
	Json::Value units;
	reader.parse(unit_file, units);
	unit_file.close();
	units = units["units"];
	for (Json::Value::iterator unit = units.begin();
		unit != units.end(); ++unit) {
		LoadUnitAttributes(*unit);
		std::string name = (*unit)["name"].asString();
		LoadUnitImages(name);
	}
}

void  ResourceManager::LoadUnitAttributes(const Json::Value &unit) {
	std::string name = unit["name"].asString();
	float collision_radius = float(unit["cradius"].asDouble());
	float selection_radius = float(unit["sradius"].asDouble());
	float speed = float(unit["mspeed"].asDouble());
	float damage = float(unit["damage"].asDouble());
	float attack_speed  = float(unit["aspeed"].asDouble());
	float range = float(unit["range"].asDouble());
	float health = float(unit["health"].asDouble());
	UnitAttributes attributes(name, collision_radius, selection_radius, speed,
			damage, attack_speed, range, health);
	unit_dictionary_.insert(std::pair<std::string, UnitAttributes>(name,
			attributes));
}

const sf::Image &ResourceManager::GetImage(const std::string &name,
										   PlayerNumber owner) const {
	switch (owner) {
		case kPlayer2:
			return unit_images_.at(name + "2");
			break;
		default:
			return unit_images_.at(name + "1");
	}
}

bool ResourceManager::LoadUnitImages(const std::string &type) {
	sf::Image image1, image2;
	image1.LoadFromFile(type + "1.png");
	image2.LoadFromFile(type + "2.png");
	unit_images_[type + "1"] = image1;
	unit_images_[type + "2"] = image2;
	return true;
}

bool ResourceManager::LoadTerrainImage(const std::string &type) {
	sf::Image image;
	image.LoadFromFile(type + ".png");
	terrain_images_[type] = image;
	return true;
}

const sf::Image &ResourceManager::GetImage(const std::string &name) const {
	return terrain_images_.at(name);
}

void ResourceManager::LoadTiles(const Json::Value &tiles) {
	for (Json::Value::iterator tile = tiles.begin(); tile != tiles.end();
			++tile) {
		std::string name = (*tile)["name"].asString();
		bool is_traversable = (*tile)["traversable"].asBool();
		traversability_[name] = is_traversable;
		LoadTerrainImage(name);
	}
}

void ResourceManager::LoadTerrain(const Vector2i &map_size,
								  const Json::Value &terrain,
								  const std::string &default_tile) {
	for (int x = 0; x < map_size.x; ++x) {
		terrain_.push_back(std::vector<std::string>());
		for (int y = 0; y < map_size.y; ++y) {
			terrain_[x].push_back(default_tile);
		}
	}
	for (Json::Value::iterator tile = terrain.begin(); tile != terrain.end();
			++tile) {
		std::string type = (*tile)["type"].asString();
		int x = (*tile)["x"].asInt();
		int y = (*tile)["y"].asInt();
		terrain_[x][y] = type;
	}
}