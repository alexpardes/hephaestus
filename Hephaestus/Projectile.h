#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "GameUnit.h"
#include "Vector2.h"

class GameState;

class Projectile : public GameObject {
	public:
    Projectile(GameState& gameState, std::shared_ptr<GameUnit> owner,
        const Vector2f &position, float direction,
        float damage, float speed);

    virtual size_t HashCode() const;
		int Id() const {return id;}
		static int serial_number_;
    virtual void PerformAction();
    const std::string &Name() const {return name;}
    bool IsAlive() const { return isAlive; }

	private:
    float CalculateDamage(const GameUnit &unitHit) const;
		std::shared_ptr<GameUnit> owner;
    Vector2f startPosition;
		float damage, speed;
		int id;
    bool isAlive;
    std::string name;
    GameState& gameState;
    SectorMap sectorMap;
};

class ProjectileModel {
	public:
		explicit ProjectileModel(const Projectile &projectile);
		ProjectileModel(const ProjectileModel &projectile1,
				const ProjectileModel &projectile2, float weight);
		Vector2f position() const {return position_;}
    float Rotation() const { return rotation; }
		int id() const {return id_;}
    const std::string &Name() const {return name;}

	private:
		Vector2f position_;
		int id_;
    float rotation;
    std::string name;
};