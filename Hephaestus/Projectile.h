#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "GameUnit.h"
#include "Vector2.h"

class GameState;

class Projectile : public GameObject {
	public:
    Projectile(GameState& gameState, const std::string &name,
        const Vector2f &position, const Vector2f& destination, float damage,
        float speed);

		int id() const {return id_;}
		static int serial_number_;
    virtual void PerformAction();
    const std::string &Name() const {return name;}
    bool IsAlive() const { return isAlive; }

	private:
		//std::shared_ptr<GameUnit> target_;
		float damage_, speed;
		int id_;
    bool isAlive;
    std::string name;
    GameState& gameState;
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