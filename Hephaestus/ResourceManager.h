#ifndef RESOURCEMANAGER_
#define RESOURCEMANAGER_

#include "GameState.h"
#include "GameUnit.h"

typedef std::unordered_map<std::string, sf::Image> ImageDictionary;
typedef std::unordered_map<std::string, bool> BoolDictionary;

class ResourceManager {
	public:
		GameState *LoadMap(const std::string &filename);
		void LoadUnitFile(const std::string &filename);
		const sf::Image &GetImage(const std::string &name,
				PlayerNumber owner) const;
		const sf::Image &GetImage(const std::string &name) const;
		const std::vector<std::vector<std::string>> &GetTerrain() const {
			return terrain_;
		}
		Vector2i GetMapSize() const {
			return Vector2i(terrain_.size(), terrain_[0].size());
		}

	private:
		void LoadUnitAttributes(const Json::Value &unit);
		bool LoadUnitImages(const std::string &type);
		void LoadTiles(const Json::Value &tiles);
		void LoadTerrain(const Vector2i &map_size, const Json::Value &terrain,
				const std::string &default_tile);
		bool LoadTerrainImage(const std::string &type);
		UnitDictionary unit_dictionary_;
		ImageDictionary unit_images_;
		ImageDictionary terrain_images_;
		BoolDictionary traversability_;
		std::vector<std::vector<std::string>> terrain_;
};

#endif