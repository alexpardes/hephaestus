#pragma once

#include "GameState.h"
#include "GameUnit.h"
#include <json/json.h>
#include "TileMap.h"

typedef std::unordered_map<std::string, sf::Texture> ImageDictionary;
typedef std::unordered_map<std::string, bool> BoolDictionary;
typedef unsigned char TerrainId;

class PathFinder;

class ResourceManager {
	public:
		GameState *LoadMap(const std::string& filename);
    const sf::Texture& GetMapImage() const { return mapImage.getTexture(); }
		const sf::Texture& GetImage(const std::string& name,
				PlayerNumber owner) const;    
		const sf::Texture& GetImage(TerrainId id) const;
    const sf::Texture& GetImage(const std::string& name) const;
		const std::vector<std::vector<TerrainId>>& GetTerrain() const {
			return terrain_;
		}

		Vector2i MapSize() const {
			return mapSize;
		}

    const sf::Font& GetDefaultFont() const {
      return defaultFont;
    }

    TileMap& FogOfWar() { return *fogTiles; }
    sf::RenderTexture& FogTexture() { return fogImage; }

	private:
    void SetupFogOfWar();
    void RenderMap();
    void SetupGameState(const Json::Value& map, GameState* state);
    void LoadUnits(const Json::Value& map);
    void LoadFonts();
    sf::Color CreateColor(std::string& rgb);
    void LoadPlayerColors(const Json::Value& map);
		void LoadUnitAttributes(const Json::Value& unit);
		bool LoadUnitImages(const Json::Value& unit);
		void LoadTiles(const Json::Value& map);
		void LoadTerrain(const Json::Value& map);
		bool LoadTerrainImage(const Json::Value& tile);
    PathFinder *LoadPathingInfo(const Json::Value& pathingInfo);

		UnitDictionary unit_dictionary_;
		std::vector<std::string> tile_table_;
		std::unordered_map<std::string, std::vector<sf::Texture>> unit_images_;
    ImageDictionary projectileImages;
		ImageDictionary terrain_images_;
		BoolDictionary traversability_;
		std::vector<std::vector<TerrainId>> terrain_;
    std::vector<sf::Color> playerColors;
    sf::Font defaultFont;
    Vector2i mapSize;
    sf::RenderTexture mapImage;

    // TODO: move this somewhere else.
    TileMap* fogTiles;
    sf::RenderTexture fogTileset;
    sf::RenderTexture fogImage;
};