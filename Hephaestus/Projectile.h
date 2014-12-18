#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "GameUnit.h"
#include "Vector2.h"

class GameState;

class Projectile : public GameObject {
	public:
    Projectile(GameState& gameState, std::shared_ptr<GameUnit> owner,
        const Vector2f &position, float direction,
        float damage, float dispersion, float speed);

    virtual size_t HashCode() const;
		int Id() const {return id;}
		static int serial_number_;
    virtual void PerformAction();
    int Type() const {return type;}
    bool IsAlive() const { return isAlive; }
    float CalculateDamage(const GameUnit &unitHit) const;
    Vector2f Origin() const;
    const std::shared_ptr<GameUnit> Owner() const {return owner;}

	private:
		std::shared_ptr<GameUnit> owner;
    Vector2f startPosition;
		float damage, speed, dispersion;
		int id, type;
    bool isAlive;
    GameState &gameState;
    SectorMap sectorMap;
};

class ProjectileModel {
	public:
		explicit ProjectileModel(const Projectile &projectile, bool isVisible);
		ProjectileModel(const ProjectileModel &projectile1,
				const ProjectileModel &projectile2, float weight);
		Vector2f Position() const {return position;}
    float Rotation() const { return rotation; }
		int Id() const {return id;}
    int Type() const {return type;}
    bool IsVisible() const {return isVisible;}
    PlayerNumber Owner() const {return owner;}

	private:
		Vector2f position;
		int id, type;
    float rotation;
    bool isVisible;
    PlayerNumber owner;
};