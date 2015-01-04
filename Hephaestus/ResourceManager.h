#pragma once
#include <json/json.h>
#include "GameState.h"
#include "GameUnit.h"
#include "Poly.h"
#include "TileMap.h"

typedef std::unordered_map<std::string, bool> BoolDictionary;
typedef unsigned char TerrainId;

class SpatialGraph;

enum UnitImageType { kUpper, kLower };

class ResourceManager {
	public:
		GameState *LoadMap(const std::string &filename);
    const sf::Texture &GetMapImage() const { return mapImage; }
		sf::Sprite UnitSprite(int unitType, PlayerNumber owner, UnitImageType type) const;
    const sf::Texture &GetImage(int projectileType) const;

		Vector2i MapSize() const {
			return mapSize;
		}

    const sf::Font &GetDefaultFont() const {
      return defaultFont;
    }

    sf::RenderTexture &FogTexture() { return fogImage; }
    const std::vector<const Poly> &Walls() const;
    const std::vector<const Poly> &DilatedWalls() const;

	private:
    struct UnitImagePair {
      sf::Texture upper, lower;      
    };

    struct UnitImageSet {
      // This maps from player number to the image pair for that player.
      std::vector<const UnitImagePair> images;
      Vector2f upperCenter, lowerCenter;
    };

    void SetupFogOfWar();
    void AddUnitsToGameState(const Json::Value &map, GameState *state);
    void LoadMapImage(const std::string &map);
    void LoadUnits(const Json::Value &map);
    void LoadFonts();
    sf::Color CreateColor(std::string &rgb);
    void LoadPlayerColors(const Json::Value &map);
		void LoadUnitAttributes(const Json::Value &unit);
		bool LoadUnitImages(const Json::Value &unit);
    std::vector<const Poly> LoadTerrain(const Json::Value &pathingInfo);

		std::vector<const UnitAttributes> unitTable;

    // This maps from the unit's type ID.
		std::vector<const UnitImageSet> unitImageTable;
    std::vector<const sf::Texture> projectileImages;
    std::vector<sf::Color> playerColors;
    sf::Font defaultFont;
    Vector2i mapSize;
    sf::Texture mapImage;
    sf::RenderTexture fogImage;
    std::vector<const Poly> polygons;
    std::vector<const Poly> dilatedPolygons;
};