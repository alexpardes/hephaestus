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

struct CollisionTestResult {
  // Will be the endpoint of the movement if there is no collision.
  Vector2f point;

  // The unit collided with, if any.
  std::shared_ptr<GameUnit> unitHit;

  CollisionTestResult(const Vector2f& point, std::shared_ptr<GameUnit> unitHit) :
      point(point), unitHit(unitHit) { }
};

class GameState {
	public:
		GameState(const UnitDictionary &unitDictionary,
				const Vector2i &mapSize, PathFinder *pathfinder);
		void AddUnit(const std::string &type, PlayerNumber owner,
				const Vector2f &location, float rotation);
		void RemoveUnit(std::shared_ptr<GameUnit> unit);
		void AddProjectile(std::shared_ptr<GameUnit> owner, const Vector2f &location,
        float direction, float damage, float speed);
		void RemoveProjectile(Projectile *projectile);
		const std::list<std::shared_ptr<GameUnit>> &Units() const {return units;}
		const std::list<Projectile *> &Projectiles() const {
			return projectiles;
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
		void AddTerrain(const Vector2f &top_left,
				const Vector2f &bottom_right);
		//const static float kPathingResolution;
		const static float kUnitGridResolution;
		std::shared_ptr<GameUnit> GetUnit(UnitId id) const;
		Vector2i map_size() const {return mapSize;}
    Vector2i GetTile(UnitId id) const;
    Vector2i GetTile(const Vector2f &gameLocation) const;
    Vector2f GetLocation(const Vector2i &gridLocation) const;
    void ExecuteTurn();
    void MoveUnit(UnitId id, Vector2f location);
    Vector2f GetUnitPosition(UnitId id) const;
    std::vector<Rect> GetWallsInRectangle(const Rect &rectangle) const;
    const std::vector<LineSegment> &GetWalls() const;

    //CollisionTestResult TestCollision(const GameUnit& unit,
    //    const Vector2f& end) const;
    //
    
    CollisionTestResult TestCollision(const Vector2f& start,
        const Vector2f& end, float radius,
        std::shared_ptr<const GameUnit> unitToIgnore = nullptr) const;
    
    // Gives the distance the unit can travel towards the end point before
    // it would collide with an obstacle.
    CollisionTestResult TestCollision(std::shared_ptr<const GameUnit> unit,
        const Vector2f& end) const;

	private:
    const PathingGrid* PathingGrid() const { return pathfinder->GetPathingGrid(); }
    void FindWalls();
    Rect GetWall(const Vector2i& tile) const;
    void GameState::AddWalls(int tileX, int tileY);
    CollisionTestResult TestWallCollision(const Vector2f& start,
      const Vector2f& end, float radius) const;

    //CollisionTestResult TestWallCollision(const GameUnit& unit,
    //    const Vector2f& end) const;

    CollisionTestResult TestUnitCollision(const Vector2f& start,
      const Vector2f& end, float radius,
      std::shared_ptr<const GameUnit> unitToIgnore = nullptr) const;

    //CollisionTestResult TestUnitCollision(const GameUnit& unit,
    //    const Vector2f& end, float radius) const;

    // Returns the closest point which is on the map.
    Vector2f Constrain(Vector2f location) const;
    Vector2i Constrain(Vector2i location) const;

    void UpdateSightMap(GameUnit& unit);

		std::list<std::shared_ptr<GameUnit>> units;
		std::list<Projectile *> projectiles;

    typedef std::vector<std::vector<std::list<std::shared_ptr<GameUnit>>>> UnitGrid;
		UnitGrid unitGrid;
		int unitGridWidth, unitGridHeight;
		float maxUnitRadius;
		const UnitDictionary &unitDefinitions;
		std::unordered_map<UnitId, std::shared_ptr<GameUnit>> unitTable;
		Vector2i mapSize;
    PathFinder *pathfinder;
    int lastUnitId;

    std::vector<LineSegment> walls;
};

class GameScene {
	public:
		GameScene(const GameState &game_state);
		GameScene(const GameScene &scene1, const GameScene &scene2,
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

		const static float kUnitGridResolution;

    std::vector<const SectorMap*> UnitViews() const { return unitViews; }

	private:
    void ComputeUnitVisibility(PlayerNumber player,
        std::vector<const SectorMap*> sightMaps);
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
    std::vector<const SectorMap*> unitViews;
    Vector2i mapSize;
};