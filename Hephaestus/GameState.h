#pragma once

#include <unordered_map>
#include "GameUnit.h"
#include "Projectile.h"
#include "Rectangle.h"
#include <PathFinding/PathFinder.h>
#include <SFML/Graphics/Shape.hpp>

class SectorMap;

const float kTileSize = 10.f;
typedef std::pair<std::string, UnitAttributes> UnitDefinition;
typedef std::unordered_map<std::string, const UnitAttributes> UnitDictionary;

class GameState {
	public:
		GameState(const UnitDictionary &unit_dictionary,
				const Vector2i &map_size, PathFinder *pathfinder);
		void AddUnit(const std::string &type, PlayerNumber owner,
				const Vector2f &location, float rotation);
		void RemoveUnit(std::shared_ptr<GameUnit> unit);
		void AddProjectile(const std::string &name, const Vector2f &location,
        std::shared_ptr<GameUnit> target, float damage, float speed);
		void RemoveProjectile(Projectile *projectile);
		const std::list<std::shared_ptr<GameUnit>> &units() const {return units_;}
		const std::list<Projectile *> &projectiles() const {
			return projectiles_;
		}
		GameState &operator=(const GameState &game_state);
		~GameState();
		void AddToPathingGrid(const std::shared_ptr<GameUnit> unit);
		void RemoveFromPathingGrid(const std::shared_ptr<GameUnit> unit);
		void AddToUnitGrid(std::shared_ptr<GameUnit> unit);
    void RemoveFromUnitGrid(const std::shared_ptr<GameUnit> unit);
		void RemoveFromUnitGrid(const std::shared_ptr<GameUnit> unit,
        const Vector2f &position);
		void UpdateUnitGrid(std::shared_ptr<GameUnit> unit,
        const Vector2f &previousPosition);
		std::vector<std::shared_ptr<GameUnit>> GetUnitsInRectangle(
        const Vector2f &corner1,
				const Vector2f &corner2) const;
    std::vector<std::shared_ptr<GameUnit>> GetUnitsInRectangle(
        const Rect& rectangle) const;
		std::vector<std::shared_ptr<GameUnit>> GetUnitsInCircle(
        const Vector2f &center, float radius) const;
		int **pathing_grid() {return pathing_grid_;}
		int pathing_width() {return pathing_width_;}
		int pathing_height() {return pathing_height_;}
		void AddTerrain(const Vector2f &top_left,
				const Vector2f &bottom_right);
		const static float kPathingResolution;
		const static float kUnitGridResolution;
		std::shared_ptr<GameUnit> GetUnit(UnitId id) const;
		Vector2i map_size() const {return map_size_;}
    Vector2i GetTile(UnitId id) const;
    Vector2i GetTile(const Vector2f &gameLocation) const;
    Vector2f GetLocation(const Vector2i &gridLocation) const;
    void ExecuteTurn();
    void MoveUnit(UnitId id, Vector2f location);
    Vector2f GetUnitPosition(UnitId id) const;
    std::vector<Rect> GetWallsInRectangle(const Rect &rectangle) const;
    std::vector<Rect> GetWalls() const;

    // Gives the distance the unit can travel towards the end point before
    // it would collide with an obstacle.
    float DistanceToObstacle(const GameUnit& unit, const Vector2f& end) const;
    float DistanceToObstacle(const GameUnit& unit, const Vector2f& end,
        float radius) const;

    std::vector<sf::ConvexShape> FindOccludedAreas(const GameUnit& unit) const;

	private:
    Rect GetWall(const Vector2i& tile) const;
    float DistanceToWall(const GameUnit& unit, const Vector2f& end,
      float radius) const;
    float DistanceToUnit(const GameUnit& unit, const Vector2f& end,
        float radius) const;

    // Returns the closest point which is on the map.
    Vector2f Constrain(Vector2f location) const;
    Vector2i Constrain(Vector2i location) const;
		void AdjustPathingGrid(const std::shared_ptr<GameUnit> unit, int value);
		std::list<std::shared_ptr<GameUnit>> units_;
		std::list<Projectile *> projectiles_;

    // TODO: change to vectors.
    typedef std::vector<std::vector<std::list<std::shared_ptr<GameUnit>>>> UnitGrid;
		UnitGrid unit_grid_;
		int unit_grid_width_, unit_grid_height_;
		int **pathing_grid_;
		int pathing_width_, pathing_height_;
		float max_unit_radius_;
		const UnitDictionary &unitDefinitions;
		std::unordered_map<UnitId, std::shared_ptr<GameUnit>> unit_table_;
		Vector2i map_size_;
    PathFinder *pathfinder;
    int lastUnitId;
    void UpdateSightMap(GameUnit& unit);
};

class GameScene {
	public:
		GameScene(GameState &game_state);
		GameScene(GameScene &scene1, GameScene &scene2,
				float weight);
		~GameScene();
		const std::list<UnitModel *> &units() const {return units_;}
		const std::list<ProjectileModel *> &projectiles() const {
			return projectiles_;
		}

    std::vector<const UnitModel *> GetUnitsInRectangle(
        const Rect& rectangle) const;
		std::vector<const UnitModel *> GetUnitsInRectangle(
				const Vector2f &corner1, const Vector2f &corner2) const;
		UnitModel *GetUnit(UnitId id) const;
    bool IsVisible(int x, int y) const { return isVisible[x][y]; }
    void ComputeVisibility(PlayerNumber playerID);

		const static float kUnitGridResolution;

	private:
		void CreateUnit(const GameUnit& unit);
		void CreateUnit(const UnitModel& unit1, const UnitModel& unit2,
				float weight);
		void CreateUnit(const UnitModel &unit);
		void AddUnit(UnitModel *unit);
		void AddToUnitGrid(const UnitModel &unit);
		std::list<UnitModel *> units_;
		std::list<ProjectileModel *> projectiles_;
		std::list<const UnitModel *> **unit_grid_;
		int unit_grid_width_, unit_grid_height_;
		float max_unit_radius_;
		std::unordered_map<UnitId, UnitModel *> unit_table_;
    std::vector<std::vector<bool>> isVisible;
    Vector2i mapSize;
};