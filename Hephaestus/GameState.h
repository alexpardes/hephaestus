#pragma once

#include <unordered_map>
#include "GameUnit.h"
#include "Projectile.h"
#include "Rectangle.h"
#include <PathFinding/PathFinder.h>
#include <SFML/Graphics/Shape.hpp>

class SectorMap;
class SpatialGraph;

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
		GameState(const std::vector<const UnitAttributes> &unitDefinitions,
				const Vector2i &mapSize, SpatialGraph *pathingGraph);

    size_t HashCode() const;
		void AddUnit(int type, PlayerNumber owner,
				const Vector2f &location, float rotation);
		void RemoveUnit(std::shared_ptr<GameUnit> unit);
		void AddProjectile(std::shared_ptr<GameUnit> owner, const Vector2f &location,
        float direction, float damage, float dispersion, float speed);
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
    void ExecuteTurn();
    void MoveUnit(UnitId id, Vector2f location);
    Vector2f GetUnitPosition(UnitId id) const;
    std::vector<Rect> GetWallsInRectangle(const Rect &rectangle) const;
    const std::vector<const Poly> &GetWalls() const;
    const SpatialGraph &PathingGraph() const { return *pathingGraph; }

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

    CollisionTestResult TestWallCollision(const Vector2f& start,
      const Vector2f& end, float radius) const;

	private:
    typedef std::vector<std::vector<std::list<std::shared_ptr<GameUnit>>>> UnitGrid;

    CollisionTestResult TestUnitCollision(const Vector2f& start,
      const Vector2f& end, float radius,
      std::shared_ptr<const GameUnit> unitToIgnore = nullptr) const;

    // Returns the closest point which is on the map.
    Vector2f Constrain(Vector2f location) const;
    void UpdateSightMap(GameUnit& unit);

		std::list<std::shared_ptr<GameUnit>> units;
		std::list<Projectile *> projectiles;
		UnitGrid unitGrid;
		int unitGridWidth, unitGridHeight;
		float maxUnitRadius;
		const std::vector<const UnitAttributes> &unitDefinitions;
		std::unordered_map<UnitId, std::shared_ptr<GameUnit>> unitTable;
		Vector2i mapSize;
    int lastUnitId;
    std::vector<const LineSegment> walls;
    SpatialGraph *pathingGraph;
};

class GameScene {
	public:
    const static float kUnitGridResolution;

    GameScene(const GameState &gameState);
		GameScene(const GameScene &scene1, const GameScene &scene2,
				float weight);
		~GameScene();
		const std::list<UnitModel *> &Units() const {return units;}
		const std::list<ProjectileModel *> &Projectiles() const {
			return projectiles;
		}

    std::vector<const UnitModel *> GetUnitsInRectangle(
        const Rect& rectangle) const;
		std::vector<const UnitModel *> GetUnitsInRectangle(
				const Vector2f &corner1, const Vector2f &corner2) const;
		UnitModel *GetUnit(UnitId id) const;
    std::vector<const SectorMap*> UnitViews() const { return unitViews; }

    const std::vector<const Poly> &walls;

	private:
    void ComputeUnitVisibility(PlayerNumber player,
        std::vector<const SectorMap*> sightMaps);
		void CreateUnit(const GameUnit& unit, bool isVisible);
		void CreateUnit(const UnitModel& unit1, const UnitModel& unit2,
				float weight);
		void CreateUnit(const UnitModel &unit);
		void AddUnit(UnitModel *unit);
		void AddToUnitGrid(const UnitModel &unit);

		std::list<UnitModel *> units;
		std::list<ProjectileModel *> projectiles;
		std::list<const UnitModel *> **unitGrid;
		int unitGridWidth, unitGridHeight;
		float maxUnitRadius;
		std::unordered_map<UnitId, UnitModel *> unitTable;
    std::vector<const SectorMap*> unitViews;
    Vector2i mapSize;
};