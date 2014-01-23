#include "stdafx.h"
#include "Projectile.h"

const float Projectile::kSpeed = .75f;
int Projectile::serial_number_ = 0;

Projectile::Projectile(const Vector2f &position,
					   GameUnit *target,
					   float damage):
		position_(position), target_(target), damage_(damage) {
	id_ = serial_number_++;
}

bool ProjectileModel::is_initialized_ = false;
//sf::Texture ProjectileModel::image_;

ProjectileModel::ProjectileModel(const Projectile &projectile) {
	position_ = projectile.position();
	id_ = projectile.id();
	if (!is_initialized_) {
		is_initialized_ = Initialize();
	}
}

bool ProjectileModel::Initialize() {
  return true;
	//return image_.loadFromFile("projectile.png");
}

ProjectileModel::ProjectileModel(const ProjectileModel &projectile1,
								 const ProjectileModel &projectile2,
								 float weight) {
	float b = weight;
	float a = 1 - weight;
	position_ = a*projectile1.position() + b*projectile2.position();
	id_ = projectile1.id();
}