#pragma once
#include <json/json.h>
#include "GameState.h"
#include "GameUnit.h"
#include "Poly.h"
#include "TileMap.h"

typedef std::unordered_map<std::string, bool> BoolDictionary;
typedef unsigned char TerrainId;

class SpatialGraph;

class ResourceManager {
	public:
		GameState *LoadMap(const std::string &filename);
    const sf::Texture &GetMapImage() const { return mapImage; }
		const sf::Texture &GetImage(int unitType, PlayerNumber owner) const;    
    const sf::Texture &GetImage(int projectileType) const;

		Vector2i MapSize() const {
			return mapSize;
		}

    const sf::Font &GetDefaultFont() const {
      return defaultFont;
    }

    sf::RenderTexture &FogTexture() { return fogImage; }

	private:
    void SetupFogOfWar();
    void SetupGameState(const Json::Value &map, GameState *state);
    void LoadMapImage();
    void LoadUnits(const Json::Value &map);
    void LoadFonts();
    sf::Color CreateColor(std::string &rgb);
    void LoadPlayerColors(const Json::Value &map);
		void LoadUnitAttributes(const Json::Value &unit);
		bool LoadUnitImages(const Json::Value &unit);
    std::vector<const Poly> LoadTerrain(const Json::Value &pathingInfo);

		std::vector<const UnitAttributes> unitTable;
		std::vector<std::vector<const sf::Texture>> unitImageTable;
    std::vector<const sf::Texture> projectileImages;
    std::vector<sf::Color> playerColors;
    sf::Font defaultFont;
    Vector2i mapSize;
    sf::Texture mapImage;
    sf::RenderTexture fogImage;
};