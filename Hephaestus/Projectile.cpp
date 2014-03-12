#include "stdafx.h"
#include "Projectile.h"
#include "Util.h"
#include "GameState.h"

int Projectile::serial_number_ = 0;

Projectile::Projectile(GameState& gameState,
                       const std::string& name,
                       const Vector2f& position,
                       const Vector2f& destination,
                       float damage,
                       float speed) : gameState(gameState) {
  this->name = name;
  //target_ = target;
  damage_ = damage;
  this->speed = speed;  
  position_ = position;  
	id_ = serial_number_++;
  isAlive = true;

  rotation_ = Util::FindAngle(destination - position);
}

void Projectile::PerformAction() {
  Vector2f v = speed * Util::MakeUnitVector(rotation_);

  CollisionTestResult collision
      = gameState.TestCollision(position_, position_ + v, 0);

  if (collision.point != position_ + v) {
    isAlive = false;

    if (collision.unitHit != nullptr) {
      collision.unitHit->modify_health(-damage_);
    }
  }

  position_ = collision.point;
}


ProjectileModel::ProjectileModel(const Projectile &projectile) {
	position_ = projectile.Position();
	id_ = projectile.id();
  name = projectile.Name();
  rotation = projectile.Rotation();
}

ProjectileModel::ProjectileModel(const ProjectileModel &projectile1,
                                 const ProjectileModel &projectile2,
                                 float weight) {
  name = projectile1.Name();
	float b = weight;
	float a = 1 - weight;
	position_ = a*projectile1.position() + b*projectile2.position();
	id_ = projectile1.id();
  rotation = Util::InterpolateAngles(projectile1.Rotation(),
      projectile2.Rotation(), weight);
}