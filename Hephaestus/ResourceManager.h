#ifndef RESOURCEMANAGER_
#define RESOURCEMANAGER_

#include "GameState.h"
#include "GameUnit.h"
#include <json/json.h>

typedef std::unordered_map<std::string, sf::Texture> ImageDictionary;
typedef std::unordered_map<std::string, bool> BoolDictionary;
typedef unsigned char TerrainId;

class PathFinder;

class ResourceManager {
	public:
		GameState *LoadMap(const std::string &filename);
		const sf::Texture &GetImage(const std::string &name,
				PlayerNumber owner) const;
		const sf::Texture &GetImage(TerrainId id) const;
    const sf::Texture &GetImage(const std::string &name) const;
		const std::vector<std::vector<TerrainId>> &GetTerrain() const {
			return terrain_;
		}
		Vector2i GetMapSize() const {
			return Vector2i(terrain_.size(), terrain_[0].size());
		}

	private:
		void LoadUnitAttributes(const Json::Value &unit);
		bool LoadUnitImages(const Json::Value &unit);
		void LoadTiles(const Json::Value &tiles);
		void LoadTerrain(const Vector2i &map_size, const Json::Value &terrain);
		bool LoadTerrainImage(const Json::Value &tile);
    PathFinder *LoadPathingInfo(const Json::Value &pathingInfo,
        int width, int height);

		UnitDictionary unit_dictionary_;
		std::vector<std::string> tile_table_;
		ImageDictionary unit_images_;
    ImageDictionary projectileImages;
		ImageDictionary terrain_images_;
		BoolDictionary traversability_;
		std::vector<std::vector<TerrainId>> terrain_;
};

#endif