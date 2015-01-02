#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include "Vector2.h"
#include "GameObject.h"
#include "UnitAttributes.h"
#include "SectorMap.h"
#include "VisibilityPolygon.h"

class Projectile;
class UnitAction;
class UnitAbility;

typedef short UnitId;

enum PlayerNumber { kPlayer1, kPlayer2 };
enum Direction { kLeft, kRight };

class GameUnit : public GameObject {
	public:
		GameUnit(UnitId id, const UnitAttributes &attributes, PlayerNumber owner,
				const Vector2f &position, float rotation);

    virtual size_t HashCode() const;
    Vector2f PreviousPosition() const;
    float Stability() const;
    int TurnsSinceHit() const;
    LineSegment SegmentFromUnit(const Vector2f &viewPoint) const;

		static std::vector<Vector2i> pathing_offsets(int x, int y) {
			return pathingOffsets[3*x + y + 4];
		}

    SectorMap &SightMap() { return sightMap; }
    const SectorMap &SightMap() const { return sightMap; }

		void ModifyHealth(float health);
    void OnAttacked(const Projectile &projectile);

		float CurrentHealth() const {return currentHealth;}

		void SetAction(UnitAction *action);
    void PerformAction();

		void Kill() {isAlive = false;}
		bool IsAlive() const {return isAlive;}
		bool operator==(const GameUnit &other) const {return this == &other;}
		bool operator!=(const GameUnit &other) const {return this != &other;}
		UnitId Id() const {return id;}
		PlayerNumber Owner() const {return owner;}
		const UnitAttributes Attributes() const {return attributes;}

    void AddAbility(UnitAbility *ability);
    UnitAbility *GetAbility(const std::string &name);
    void SetIdleAbility(UnitAbility* ability) { idleAbility = ability; }
    Direction Facing() const { return facing; }
    void SetFacing(Direction facing) { this->facing = facing; }

	private:
		bool isAlive;
    SectorMap sightMap;
    Vector2f previousPosition;
    float currentHealth, previousRotation, stability;
    int turnsSinceHit;
    std::vector<Vector2f> visiblePoints;
		static std::vector<Vector2i>* pathingOffsets;
    Direction facing;
		UnitId id;
		UnitAttributes attributes;
		PlayerNumber owner;
    UnitAction *action;
    std::unordered_map<std::string, UnitAbility*> abilities;
    UnitAbility* idleAbility;
};
typedef std::list<std::shared_ptr<GameUnit>> UnitList;

class UnitModel {
	public:
		explicit UnitModel(const GameUnit &unit);
		UnitModel(const UnitModel &unit1, const UnitModel &unit2,
				float weight);
		float CurrentHealth() const { return currentHealth; }
    float MaxHealth() const { return maxHealth; }
		float Radius() const { return radius; }
		Vector2f Position() const {return position;}
		float Rotation() const {return rotation;}
    float Stability() const {return stability;}
		PlayerNumber Owner() const {return owner;}
		int Type() const {return type;}
		int Id() const {return id;}
    bool IsVisible() const { return isVisible; }
    void SetVisible(bool visible) { isVisible = visible; }
    const VisibilityPolygon &Visibility() const { return visibility; }
    Direction Facing() const { return facing; }

	private:
		Vector2f position;
		float rotation, currentHealth, maxHealth, radius, stability;
		int id, type;
		PlayerNumber owner;
    VisibilityPolygon visibility;
    bool isVisible;
    Direction facing;
};