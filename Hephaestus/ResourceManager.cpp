#include "stdafx.h"
#include "Coloring.h"
#include "ResourceManager.h"
#include "MinkowskiSum.h"
#include <PathFinding/SpatialGraph.h>

float FindMaxUnitSize(const std::vector<const UnitAttributes> &unitTable) {
  float maxSize = 0.f;
  for (auto unit : unitTable) {
    maxSize = std::max(unit.CollisionRadius(), maxSize);
  }
  return maxSize;
}

GameState *ResourceManager::LoadMap(const std::string &filename) {
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
  LoadMapImage(filename.substr(0, filename.length() - 4));

  polygons = LoadTerrain(map["polygons"]);
  Poly border;
  border.Add(Vector2f(0, 0));
  border.Add(Vector2f(0, (float) mapSize.y));
  border.Add(Vector2f((float) mapSize.x, (float) mapSize.y));
  border.Add(Vector2f((float) mapSize.x, 0));
  border.SetReversed(true);
  polygons.push_back(border);

  dilatedPolygons = MinkowskiSum::Dilate(polygons, FindMaxUnitSize(unitTable));

  // TODO: Combining intersections before dilation currently causes bad results for some reason.
  MinkowskiSum::CombineIntersections(polygons);
  auto spatialGraph = new SpatialGraph(dilatedPolygons);
	auto state = new GameState(unitTable, mapSize, spatialGraph, polygons, dilatedPolygons);

  AddUnitsToGameState(map, state);
	return state;
}

const std::vector<const Poly> &ResourceManager::Walls() const {
  return polygons;
}

const std::vector<const Poly> &ResourceManager::DilatedWalls() const {
  return dilatedPolygons;
}

void ResourceManager::LoadMapImage(const std::string &map) {
  mapImage.loadFromFile(map + ".png");
}

void ResourceManager::SetupFogOfWar() {
  fogImage.create(mapSize.x, mapSize.y);
  fogImage.setSmooth(true);

  // A RenderContext must be deactivated before using in another thread.
  fogImage.setActive(false);
}

void ResourceManager::LoadUnits(const Json::Value &map) {
  unitTable.clear();
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
sf::Color ResourceManager::CreateColor(std::string &rgb) {
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

void ResourceManager::LoadPlayerColors(const Json::Value &map) {
  Json::Value players = map["players"];
  for (Json::Value playerColor : players) {
    sf::Color color = CreateColor(playerColor.asString());
    playerColors.push_back(color);
  }
}

std::vector<const Poly> ResourceManager::LoadTerrain(const Json::Value &jsonPolygons) {
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

void ResourceManager::LoadUnitAttributes(const Json::Value &unit) {
	std::string name = unit["Name"].asString();
  int type = unit["Index"].asInt();
	float collisionRadius = float(unit["Collision Radius"].asDouble());
	float selectionRadius = float(unit["Selection Radius"].asDouble());
	float speed = float(unit["Move Speed"].asDouble());
	float damage = float(unit["Damage"].asDouble());
  float dispersion = Util::Radians((float) unit["Dispersion"].asDouble());
	float attackSpeed  = float(unit["Attack Speed"].asDouble());
  float projectileSpeed = (float) unit["Projectile Speed"].asDouble();
  float range = 1e6f;
	float health = float(unit["Health"].asDouble());
  float healthRegen = (float) unit["Health Regen"].asDouble();
  float regenDelay = (float) unit["Regen Delay"].asDouble();
  float moveInstability = (float) unit["Movement Penalty"].asDouble();
  float rotateInstability = (float) unit["Rotation Penalty"].asDouble();
  float stabilityRecovery = (float) unit["Accuracy Recovery"].asDouble();
  float aimWidth = (float) unit["Aim Point"].asDouble();
	UnitAttributes attributes(type, collisionRadius, selectionRadius, speed,
			damage, attackSpeed, range, dispersion, projectileSpeed, health,
      healthRegen, regenDelay, moveInstability, rotateInstability,
      stabilityRecovery, aimWidth);

	unitTable.push_back(attributes);
}

sf::Sprite ResourceManager::UnitSprite(int unitType,
                                       PlayerNumber owner,
                                       UnitImageType type) const {
  bool isUpper = type == kUpper;                                         
  auto &imagePair = unitImageTable[unitType].images[owner];

  auto sprite = sf::Sprite(isUpper ? imagePair.upper : imagePair.lower);
  sprite.setOrigin(isUpper ? unitImageTable[unitType].upperCenter : unitImageTable[unitType].lowerCenter);
  return sprite;
}

const sf::Texture &ResourceManager::GetImage(int projectileType) const {
  return projectileImages[projectileType];
}

bool ResourceManager::LoadUnitImages(const Json::Value& unit) {
	std::string type = unit["Name"].asString();
	std::string upperFilename = unit["Upper Image"].asString();
	std::string upperMaskFilename = unit["Color Mask"].asString();
  std::string lowerFilename = unit["Lower Image"].asString();
  std::string projectileSource = unit["Projectile"].asString();

  sf::Image upperBase, upperMask, lowerBase;
	upperBase.loadFromFile(upperFilename);
	upperMask.loadFromFile(upperMaskFilename);
  lowerBase.loadFromFile(lowerFilename);
  if (upperMask.getSize() != upperBase.getSize())
    upperMask = upperBase;


  UnitImageSet imageSet;
  imageSet.upperCenter = Vector2f(
    upperBase.getSize().x * 0.4f,
    upperBase.getSize().y * 0.5f);

  imageSet.lowerCenter = Vector2f(
    lowerBase.getSize().x * 0.5f,
    lowerBase.getSize().y * 0.5f);

  for (sf::Color color : playerColors) {
    sf::Texture upperImage;
    upperImage.loadFromImage(Coloring::ColorImage(upperBase, upperMask, color));
    upperImage.setSmooth(true);

    sf::Texture lowerImage;
    lowerImage.loadFromImage(lowerBase);
    lowerImage.setSmooth(true);    

    UnitImagePair imagePair;
    imagePair.upper = upperImage;
    imagePair.lower = lowerImage;
    imageSet.images.push_back(imagePair);    
  }
  unitImageTable.push_back(imageSet);

  sf::Texture projectileImage;
  projectileImage.loadFromFile(projectileSource);
  projectileImage.setSmooth(true);
  projectileImages.push_back(projectileImage);
	return true;
}

void ResourceManager::AddUnitsToGameState(const Json::Value &map,
                                          GameState *state) {

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
