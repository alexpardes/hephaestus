#ifndef GAMESTATE_
#define GAMESTATE_

#include "GameUnit.h"
#include "Projectile.h"


const float kTileSize = 50.f;
typedef std::unordered_map<std::string, const UnitAttributes> UnitDictionary;

class GameState {
	public:
		GameState(const UnitDictionary &unit_dictionary,
				const Vector2i &map_size);
		void AddUnit(const std::string &type, PlayerNumber owner,
				const Vector2f &location, float rotation);
		void RemoveUnit(GameUnit *unit);
		void AddProjectile(const Vector2f &location,
				GameUnit *target, float damage);
		void RemoveProjectile(Projectile *projectile);
		const std::list<GameUnit *> &units() const {return units_;}
		const std::list<Projectile *> &projectiles() const {
			return projectiles_;
		}
		GameState &operator=(const GameState &game_state);
		~GameState();
		void AddToPathingGrid(const GameUnit &unit);
		void RemoveFromPathingGrid(const GameUnit &unit);
		void AddToUnitGrid(GameUnit &unit);
		void RemoveFromUnitGrid(GameUnit &unit);
		void UpdateUnitGrid(GameUnit &unit);
		std::list<GameUnit *> GetUnitsInRectangle(const Vector2f &corner1,
				const Vector2f &corner2) const;
		std::list<GameUnit *> GetUnitsInCircle(const Vector2f &center,
				float radius) const;
		int **pathing_grid() {return pathing_grid_;}
		int pathing_width() {return pathing_width_;}
		int pathing_height() {return pathing_height_;}
		void AddTerrain(const Vector2f &top_left,
				const Vector2f &bottom_right);
		const static float kPathingResolution;
		const static float kUnitGridResolution;
		GameUnit *GetUnit(unitId id) const;
		Vector2i map_size() const {return map_size_;}

	private:
		void AdjustPathingGrid(const GameUnit &unit, int value);
		std::list<GameUnit *> units_;
		std::list<Projectile *> projectiles_;
		std::list<GameUnit *> **unit_grid_;
		int unit_grid_width_, unit_grid_height_;
		int **pathing_grid_;
		int pathing_width_, pathing_height_;
		float max_unit_radius_;
		const UnitDictionary &unit_dictionary_;
		std::unordered_map<unitId, GameUnit *> unit_table_;
		Vector2i map_size_;
};

class GameScene {
	public:
		explicit GameScene(GameState &game_state);
		GameScene(GameScene &scene1, GameScene &scene2,
				float weight);
		~GameScene();
		const std::list<UnitModel *> &units() const {return units_;}
		const std::list<ProjectileModel *> &projectiles() const {
			return projectiles_;
		}

		std::list<const UnitModel *> GetUnitsInRectangle(
				const Vector2f &corner1, const Vector2f &corner2) const;
		UnitModel *GetUnit(unitId id) const;
		const static float kUnitGridResolution;

	private:
		void CreateUnit(const GameUnit &unit);
		void CreateUnit(const UnitModel &unit1, const UnitModel &unit2,
				float weight);
		void CreateUnit(const UnitModel &unit);
		void AddUnit(UnitModel *unit);
		void AddToUnitGrid(const UnitModel &unit);
		std::list<UnitModel *> units_;
		std::list<ProjectileModel *> projectiles_;
		std::list<const UnitModel *> **unit_grid_;
		int unit_grid_width_, unit_grid_height_;
		float max_unit_radius_;
		std::unordered_map<unitId, UnitModel *> unit_table_;
};

#endif