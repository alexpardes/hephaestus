#include "stdafx.h"
#include "Coloring.h"
#include "ResourceManager.h"
#include <PathFinding/SpatialGraph.h>

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

  SetupFogOfWar();
  LoadPlayerColors(map);
  LoadUnits(map);
  LoadMapImage();

  auto polygons = LoadTerrain(map["polygons"]);
  Poly border;
  border.Add(Vector2f(0, 0));
  border.Add(Vector2f(0, (float) mapSize.y));
  border.Add(Vector2f((float) mapSize.x, (float) mapSize.y));
  border.Add(Vector2f((float) mapSize.x, 0));
  polygons.push_back(border);
  auto spatialGraph = new SpatialGraph(polygons);
	auto state = new GameState(unitTable, mapSize, spatialGraph);

  SetupGameState(map, state);
	return state;
}

void ResourceManager::LoadMapImage() {
  mapImage.loadFromFile("default.png");
}

void ResourceManager::SetupFogOfWar() {
  fogImage.create(mapSize.x, mapSize.y);
  fogImage.setSmooth(true);
}

void ResourceManager::LoadUnits(const Json::Value& map) {
  Json::Value types = map["types"];
  for (auto type : types) {
      LoadUnitAttributes(type);
      LoadUnitImages(type);
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

std::vector<const Poly> ResourceManager::LoadTerrain(const Json::Value& jsonPolygons) {
  std::vector<const Poly> polygons;
  for(auto jsonPolygon : jsonPolygons) {
    Poly polygon;
    for (auto vertex : jsonPolygon) {
      polygon.Add(Vector2f((float) vertex[0U].asDouble(), (float) vertex[1].asDouble()));
    }
    polygons.push_back(polygon);
  }
  return polygons;
}

void ResourceManager::LoadUnitAttributes(const Json::Value& unit) {
	std::string name = unit["Name"].asString();
  int type = unit["Index"].asInt();
	float collision_radius = float(unit["Collision Radius"].asDouble());
	float selection_radius = float(unit["Selection Radius"].asDouble());
	float speed = float(unit["Move Speed"].asDouble());
	float damage = float(unit["Damage"].asDouble());
	float attack_speed  = float(unit["Attack Speed"].asDouble());
	//float range = float(unit["range"].asDouble());
  float range = 1e6f;
	float health = float(unit["Health"].asDouble());
	UnitAttributes attributes(type, collision_radius, selection_radius, speed,
			damage, attack_speed, range, health);
	unitTable.push_back(attributes);
}

const sf::Texture& ResourceManager::GetImage(int unitType,
										   PlayerNumber owner) const {
	switch (owner) {
		case kPlayer2:
			return unitImageTable[unitType][1];
		default:
			return unitImageTable[unitType][0];
	}
}

const sf::Texture& ResourceManager::GetImage(int projectileType) const {
  return projectileImages[projectileType];
}

bool ResourceManager::LoadUnitImages(const Json::Value& unit) {
	std::string type = unit["Name"].asString();
	std::string source1 = unit["Image"].asString();
	std::string source2 = unit["Color Mask"].asString();
  std::string projectileSource = unit["Projectile"].asString();

  sf::Image base, mask;
	base.loadFromFile(source1);
	mask.loadFromFile(source2);

  unitImageTable.push_back(std::vector<const sf::Texture>());
  for (sf::Color color : playerColors) {
    sf::Texture unitImage;
    unitImage.loadFromImage(Coloring::ColorImage(base, mask, color));
    unitImage.setSmooth(true);
    unitImageTable.back().push_back(unitImage);
  }

  sf::Texture projectileImage;
  projectileImage.loadFromFile(projectileSource);
  projectileImage.setSmooth(true);
  projectileImages.push_back(projectileImage);
	return true;
}

void ResourceManager::SetupGameState(const Json::Value& map,
                                     GameState* state) {

  Json::Value units = map["units"];
  for (auto unit : units) {
    int unitType = unit["type"].asInt();
    int owner = unit["owner"].asInt();
    auto position = unit["position"];
    float x = float(position[0U].asDouble());
    float y = float(position[1].asDouble());
    float rotation = float((unit)["rotation"].asDouble());
    state->AddUnit(unitType, PlayerNumber(owner), Vector2f(x, y), rotation);
  }
}
