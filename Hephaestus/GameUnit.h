#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include "Vector2.h"
#include "GameObject.h"
#include "UnitAttributes.h"
#include "SectorMap.h"

class UnitAction;
class UnitAbility;

typedef short UnitId;

enum PlayerNumber { kPlayer1, kPlayer2 };

class GameUnit : public GameObject {
	public:
		GameUnit(UnitId id, const UnitAttributes &attributes, PlayerNumber owner,
				const Vector2f &position, float rotation);

		Vector2f Position() const {return position_;}

		static std::vector<Vector2i> pathing_offsets(int x, int y) {
			return pathing_offsets_[3*x + y + 4];
		}

    SectorMap& SightMap() { return sightMap; }
    const SectorMap& SightMap() const { return sightMap; }

		void modify_health(float health) {
			current_health_ += health;
			if (current_health_ < 0) current_health_ = 0;
			if (current_health_ > attributes_.max_health()) {
				current_health_ = attributes_.max_health();
			}
		}

		float current_health() const {return current_health_;}

		void SetAction(UnitAction *action);
    void PerformAction();

		void Kill() {is_alive_ = false;}
		bool IsAlive() const {return is_alive_;}
		bool operator==(const GameUnit &other) const {return this == &other;}
		bool operator!=(const GameUnit &other) const {return this != &other;}
		UnitId Id() const {return id_;}
		PlayerNumber Owner() const {return owner_;}
		const UnitAttributes Attributes() const {return attributes_;}

    void AddAbility(UnitAbility *ability);
    UnitAbility *GetAbility(const std::string &name);

	private:
		bool is_alive_;
    SectorMap sightMap;
		static std::vector<Vector2i>* pathing_offsets_;
		float current_health_;
		UnitId id_;
		UnitAttributes attributes_;
		PlayerNumber owner_;
    UnitAction *action;
    std::unordered_map<std::string, UnitAbility*> abilities;
};
typedef std::list<std::shared_ptr<GameUnit>> UnitList;

class UnitModel {
	public:
		explicit UnitModel(const GameUnit &unit);
		UnitModel(const UnitModel &unit1, const UnitModel &unit2,
				float weight);
		float current_health() const {return current_health_;}
		float radius() const {return radius_;}
		Vector2f position() const {return position_;}
		void set_position(const Vector2f &position) {position_ = position;}
		void set_rotation(float rotation) {rotation_ = rotation;}
		float rotation() const {return rotation_;}
		PlayerNumber Owner() const {return owner_;}
		const std::string &Name() const {return name;}
		int Id() const {return id_;}
    void SetSightMap(SectorMap* map) { sightMap = map; }
    const SectorMap* SightMap() const { return sightMap; }

	private:
		Vector2f position_;
		float rotation_;
		float current_health_;
		float radius_;
		int id_;
		PlayerNumber owner_;
		std::string name;
    SectorMap* sightMap;
};