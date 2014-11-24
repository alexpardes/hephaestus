#pragma once
#include <json/json.h>
#include "GameState.h"
#include "GameUnit.h"
#include "Poly.h"
#include "TileMap.h"

typedef std::unordered_map<std::string, sf::Texture> ImageDictionary;
typedef std::unordered_map<std::string, bool> BoolDictionary;
typedef unsigned char TerrainId;

class SpatialGraph;

class ResourceManager {
	public:
		GameState *LoadMap(const std::string &filename);
    const sf::Texture &GetMapImage() const { return mapImage; }
		const sf::Texture &GetImage(const std::string &name,
				PlayerNumber owner) const;    
    const sf::Texture &GetImage(const std::string &name) const;

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

		UnitDictionary unitDictionary;
		std::unordered_map<std::string, std::vector<sf::Texture>> unitImages;
    ImageDictionary projectileImages;
    std::vector<sf::Color> playerColors;
    sf::Font defaultFont;
    Vector2i mapSize;
    sf::Texture mapImage;
    sf::RenderTexture fogImage;
};