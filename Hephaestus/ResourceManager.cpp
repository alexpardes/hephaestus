#include "stdafx.h"
#include "ResourceManager.h"
#include "PathFinder.h"
#include <PathFinding/SubgoalPathFinder.h>
#include <PathFinding/VectorPathingGrid.h>

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

	int map_width = map["width"].asInt();
	int map_height = map["height"].asInt();
	Json::Value terrain = map["terrain"];
	LoadTerrain(Vector2i(map_width, map_height), terrain);
  PathFinder *pathfinder = LoadPathingInfo(map["pathinginfo"], map_width, map_height);
	GameState *state = new GameState(unit_dictionary_,
      Vector2i(map_width, map_height),
      pathfinder);

	Json::Value types = map["types"];
	for (Json::Value::iterator type = types.begin(); type != types.end();
			++type) {
		LoadUnitAttributes(*type);
		LoadUnitImages(*type);
	}

	Json::Value units = map["units"];
	for (Json::Value::iterator unit = units.begin(); unit != units.end();
			++unit) {
		std::string unit_type = (*unit)["type"].asString();
		int owner = (*unit)["owner"].asInt();
		float x = float((*unit)["x"].asDouble());
		float y = float((*unit)["y"].asDouble());
		//float rotation = float((*unit)["rotation"].asDouble());
		float rotation = 0; // Rotation needs to be added to the map editor.
		state->AddUnit(unit_type, PlayerNumber(owner), Vector2f(x, y), rotation);
	}

	for (int x = 0; x < map_width; ++x) {
		for (int y = 0; y < map_height; ++y) {
			if (!traversability_[tile_table_.at(terrain_[x][y])]) {
				Vector2f top_left(x*kTileSize, y*kTileSize);
				Vector2f bottom_right = top_left + Vector2f(kTileSize, kTileSize);
				state->AddTerrain(top_left, bottom_right);
			}
		}
	}

	return state;
}

PathFinder *ResourceManager::LoadPathingInfo(const Json::Value &pathingInfo,
                                             int width,
                                             int height) {

  PathingGrid *grid = new VectorPathingGrid(width, height);
  for (size_t x = 0; x < width; ++x) {
    for (size_t y = 0; y < height; ++y) {
      grid->SetBlocked(x, y, !traversability_[tile_table_.at(terrain_[x][y])]);
    }
  }
  std::vector<const Vector2i> subgoals;
  Json::Value::const_iterator subgoal = pathingInfo["subgoals"].begin();
  while (subgoal != pathingInfo["subgoals"].end()) {    
    Json::Value::const_iterator coordinate = (*subgoal++).begin();
    int x = (*coordinate).asInt();
    ++coordinate;
    int y = (*coordinate).asInt();
    subgoals.push_back(Vector2i(x, y));
  }
  std::vector<const std::vector<int>> adjacencyLists;
  Json::Value::const_iterator adjacencyList = pathingInfo["adjacencies"].begin();
  while (adjacencyList != pathingInfo["adjacencies"].end()) {
    Json::Value::const_iterator it = (*adjacencyList).begin();
    std::vector<int> neighbors;
    while (it != (*adjacencyList).end()) {
      neighbors.push_back((*it++).asInt());
    }
    adjacencyLists.push_back(neighbors);
    ++adjacencyList;
  }

  return new SubgoalPathFinder(grid, subgoals, adjacencyLists);
}

// Can be removed.
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
		LoadUnitImages(*unit);
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

const sf::Texture &ResourceManager::GetImage(const std::string &name,
										   PlayerNumber owner) const {
	switch (owner) {
		case kPlayer2:
			return unit_images_.at(name + "2");
			break;
		default:
			return unit_images_.at(name + "1");
	}
}

bool ResourceManager::LoadUnitImages(const Json::Value &unit) {
	std::string type = unit["name"].asString();
	std::string source1 = unit["source1"].asString();
	std::string source2 = unit["source2"].asString();

	sf::Texture image1, image2;
	image1.loadFromFile(source1);
	image2.loadFromFile(source2);
	unit_images_[type + "1"] = image1;
	unit_images_[type + "2"] = image2;
	return true;
}

bool ResourceManager::LoadTerrainImage(const Json::Value &tile) {
	std::string type = tile["name"].asString();
	std::string source = tile["source"].asString();
	sf::Texture image;
	image.loadFromFile(source);
	terrain_images_[type] = image;
	return true;
}

const sf::Texture &ResourceManager::GetImage(terrainId id) const {	
	return terrain_images_.at(tile_table_.at(id));
}

void ResourceManager::LoadTiles(const Json::Value &tiles) {
	for (Json::Value::iterator tile = tiles.begin(); tile != tiles.end();
			++tile) {
		std::string name = (*tile)["name"].asString();
		bool is_traversable = (*tile)["traversable"].asBool();
		tile_table_.push_back(name);
		traversability_[name] = is_traversable;
		LoadTerrainImage(*tile);
	}
}

void ResourceManager::LoadTerrain(const Vector2i &map_size,
								  const Json::Value &terrain) {
	for (int x = 0; x < map_size.x; ++x) {
		terrain_.push_back(std::vector<terrainId>());
		for (int y = 0; y < map_size.y; ++y) {
			terrain_[x].push_back(terrain[x][y].asInt());
		}
	}
}