#include "stdafx.h"
#include "Coloring.h"
#include "ResourceManager.h"
#include <PathFinding/SpatialGraph.h>
#include <PathFinding/SubgoalPathFinder.h>
#include <PathFinding/Subgoal.h>
#include <PathFinding/VectorPathingGrid.h>

GameState *ResourceManager::LoadMap(const std::string& filename) {
  LoadFonts();

	std::ifstream mapFile(filename);
	if (!mapFile.good()) {
		assert(false);
		return nullptr;
	}
	Json::Reader reader;
	Json::Value map;
	reader.parse(mapFile, map);
	mapFile.close();

  mapSize.x = map["width"].asInt();
  mapSize.y = map["height"].asInt();

  LoadTiles(map);
  LoadTerrain(map);
  RenderMap();
  SetupFogOfWar();
  LoadPlayerColors(map);
  LoadUnits(map);

  PathFinder *pathfinder = LoadPathingInfo(map["pathinginfo"]);
  SpatialGraph *pathingGraph = LoadSpatialGraph(map["pathinginfo"]);
	GameState *state = new GameState(unit_dictionary_, mapSize, pathfinder, pathingGraph);

  SetupGameState(map, state);

	return state;
}

void ResourceManager::SetupFogOfWar() {
  fogImage.create(int(mapSize.x * kTileSize), int(mapSize.y * kTileSize));
  fogImage.setSmooth(true);
  sf::RectangleShape emptyTile(Vector2f(kTileSize, kTileSize));
  emptyTile.setPosition(0, 0);
  emptyTile.setFillColor(sf::Color(0, 0, 0, 0));
  sf::Color fogColor(0, 0, 0, 40);
  sf::RectangleShape fogTile(Vector2f(kTileSize, kTileSize));
  fogTile.setPosition(kTileSize, 0);
  fogTile.setFillColor(fogColor);

  fogTileset.create(int(2 * kTileSize), int(kTileSize));
  fogTileset.draw(emptyTile);
  fogTileset.draw(fogTile);

  fogTileset.display();

  fogTiles = new TileMap(mapSize, kTileSize);
  fogTiles->SetTileset(fogTileset.getTexture());
}

// TODO: handle maps that are larger than the max texture size.
void ResourceManager::RenderMap() {
  mapImage.create(int(kTileSize * mapSize.x), int(kTileSize * mapSize.y));
  for (int x = 0; x < mapSize.x; ++x) {
    for (int y = 0; y < mapSize.y; ++y) {
      const sf::Texture &terrain_image = GetImage(terrain_[x][y]);
      sf::Sprite tileSprite;
      tileSprite.setTexture(terrain_image);
      Vector2f position = Vector2f(x*kTileSize, y*kTileSize);
      tileSprite.setPosition(position);
      mapImage.draw(tileSprite);
    }
  }

  mapImage.display();
  terrain_images_.clear();
}

void ResourceManager::LoadUnits(const Json::Value& map) {
  Json::Value types = map["types"];
  for (Json::Value::iterator type = types.begin(); type != types.end();
    ++type) {
      LoadUnitAttributes(*type);
      LoadUnitImages(*type);
  }
}

void ResourceManager::LoadFonts() {
  defaultFont.loadFromFile("fonts/DejaVuSans.ttf");
}

// Assumes hexadecimal string formatted as 0xRRGGBBAA.
sf::Color ResourceManager::CreateColor(std::string& rgb) {
  int red, green, blue, alpha;
  std::stringstream stringStream;
  stringStream << std::hex << rgb.substr(2, 2);
  stringStream >> red;
  stringStream.clear();
  stringStream << std::hex << rgb.substr(4, 2);
  stringStream >> green;
  stringStream.clear();
  stringStream << std::hex << rgb.substr(6, 2);
  stringStream >> blue;
  stringStream.clear();
  stringStream << std::hex << rgb.substr(8, 2);
  stringStream >> alpha;
  return sf::Color(red, green, blue, alpha);
}

void ResourceManager::LoadPlayerColors(const Json::Value& map) {
  Json::Value players = map["players"];
  for (Json::Value playerColor : players) {
    sf::Color color = CreateColor(playerColor.asString());
    playerColors.push_back(color);
  }
}

PathFinder *ResourceManager::LoadPathingInfo(const Json::Value& pathingInfo) {

  PathingGrid *grid = new VectorPathingGrid(mapSize);
  for (int x = 0; x < mapSize.x; ++x) {
    for (int y = 0; y < mapSize.y; ++y) {
      grid->SetBlocked(x, y, !traversability_[tile_table_.at(terrain_[x][y])]);
    }
  }
  std::vector<Subgoal*> subgoals;
  Json::Value::const_iterator subgoal = pathingInfo["subgoals"].begin();
  while (subgoal != pathingInfo["subgoals"].end()) {    
    Json::Value::const_iterator coordinate = (*subgoal++).begin();
    int x = (*coordinate++).asInt();
    int y = (*coordinate++).asInt();

    Vector2i direction;
    switch ((*coordinate).asInt()) {
      case 0:
        direction = Vector2i(-1, -1);
        break;
      case 1:
        direction = Vector2i(1, -1);
        break;
      case 2:
        direction = Vector2i(-1, 1);
        break;
      case 3:
        direction = Vector2i(1, 1);
        break;
      default:
        assert(false);
    }
    subgoals.push_back(new Subgoal(Vector2i(x, y), direction));
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

  return new SubgoalPathFinder(grid, Vector2f(kTileSize, kTileSize),
      subgoals, adjacencyLists);
}

SpatialGraph *ResourceManager::LoadSpatialGraph(const Json::Value& pathingInfo) {
  std::vector<Vector2f> points;
  Json::Value::const_iterator subgoal = pathingInfo["subgoals"].begin();
  while (subgoal != pathingInfo["subgoals"].end()) {    
    Json::Value::const_iterator coordinate = (*subgoal++).begin();
    int x = (*coordinate++).asInt();
    int y = (*coordinate++).asInt();
    Vector2i direction;
    switch ((*coordinate).asInt()) {
    case 0:
      break;
    case 1:
      ++x;
      break;
    case 2:
      ++y;
      break;
    case 3:
      ++x;
      ++y;
      break;
    default:
      assert(false);
    }

    points.push_back(Vector2f(float(kTileSize * x), float(kTileSize * y)));
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

  return new SpatialGraph(points, adjacencyLists);
}


void ResourceManager::LoadUnitAttributes(const Json::Value& unit) {
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

const sf::Texture& ResourceManager::GetImage(const std::string& name,
										   PlayerNumber owner) const {
	switch (owner) {
		case kPlayer2:
			return unit_images_.at(name)[1];
		default:
			return unit_images_.at(name)[0];
	}
}

const sf::Texture& ResourceManager::GetImage(const std::string& name) const {
  return projectileImages.at(name);
}

bool ResourceManager::LoadUnitImages(const Json::Value& unit) {
	std::string type = unit["name"].asString();
	std::string source1 = unit["source1"].asString();
	std::string source2 = unit["source2"].asString();
  std::string projectileSource = unit["projectilesource"].asString();

  sf::Image base, mask;
	base.loadFromFile(source1);
	mask.loadFromFile(source2);

  for (sf::Color color : playerColors) {
    sf::Texture unitImage;
    unitImage.loadFromImage(Coloring::ColorImage(base, mask, color));
    unitImage.setSmooth(true);
    unit_images_[type].push_back(unitImage);
  }

  sf::Texture projectileImage;
  projectileImage.loadFromFile(projectileSource);
  projectileImage.setSmooth(true);
  projectileImages[type] = projectileImage;
	return true;
}

bool ResourceManager::LoadTerrainImage(const Json::Value& tile) {
	std::string type = tile["name"].asString();
	std::string source = tile["source"].asString();
	sf::Texture image;
	image.loadFromFile(source);
	terrain_images_[type] = image;

	return true;
}

const sf::Texture& ResourceManager::GetImage(TerrainId id) const {	
	return terrain_images_.at(tile_table_.at(id));
}

void ResourceManager::LoadTiles(const Json::Value& map) {
  Json::Value tiles = map["tiles"];

  int tileNumber = 0;
	for (Json::Value tile : tiles) {    
		std::string name = tile["name"].asString();
		bool is_traversable = tile["traversable"].asBool();
		tile_table_.push_back(name);
		traversability_[name] = is_traversable;
		LoadTerrainImage(tile);
    ++tileNumber;
	}
}

void ResourceManager::LoadTerrain(const Json::Value& map) {
	Json::Value terrain = map["terrain"];
	for (int x = 0; x < mapSize.x; ++x) {
		terrain_.push_back(std::vector<TerrainId>());
		for (int y = 0; y < mapSize.y; ++y) {
			terrain_[x].push_back(terrain[x][y].asInt());
		}
	}
}

void ResourceManager::SetupGameState(const Json::Value& map,
                                     GameState* state) {
  for (int x = 0; x < mapSize.x; ++x) {
    for (int y = 0; y < mapSize.y; ++y) {
      if (!traversability_[tile_table_.at(terrain_[x][y])]) {
        Vector2f top_left(x*kTileSize, y*kTileSize);
        Vector2f bottom_right = top_left + Vector2f(kTileSize, kTileSize);
        //state->AddTerrain(top_left, bottom_right);
      }
    }
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
}