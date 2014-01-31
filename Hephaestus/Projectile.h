#ifndef PROJECTILE_
#define PROJECTILE_

#include <SFML/Graphics/Texture.hpp>
#include "GameUnit.h"
#include "Vector2.h"

class Projectile {
	public:
		Projectile(const Vector2f &position, GameUnit *target, float damage);
		GameUnit *target() {return target_;}
		void set_position(const Vector2f &position) {position_ = position;}
		void Move(const Vector2f &movement) {position_ += movement;}
		float damage() const {return damage_;}
		Vector2f position() const {return position_;}
		int id() const {return id_;}
		static const float kSpeed;
		static int serial_number_;

	private:
		Vector2f position_;
		GameUnit *target_;
		float damage_;
		int id_;
};

class ProjectileModel {
	public:
		explicit ProjectileModel(const Projectile &projectile);
		ProjectileModel(const ProjectileModel &projectile1,
				const ProjectileModel &projectile2, float weight);
		Vector2f position() const {return position_;}
		int id() const {return id_;}
		static const sf::Texture &image() {
      return sf::Texture();
      //return image_;
    }

	private:
		bool Initialize();
		Vector2f position_;
		int id_;
		static bool is_initialized_;
		//static sf::Texture image_;
};

#endif