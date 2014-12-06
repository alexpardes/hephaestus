#include "stdafx.h"
#include "Projectile.h"
#include "Util.h"
#include "GameState.h"

int Projectile::serial_number_ = 0;

Projectile::Projectile(GameState &gameState,
                       std::shared_ptr<GameUnit> owner,
                       const Vector2f &position,
                       float direction,
                       float damage,
                       float speed) : gameState(gameState) {
  this->owner = owner;
  type = owner->Attributes().Type();
  sectorMap = owner->SightMap();
  this->damage = damage;
  this->speed = speed;  
  this->position = position;
  startPosition = position;
	id = serial_number_++;
  isAlive = true;
  rotation = direction;
}

size_t Projectile::HashCode() const {
  size_t hash = GameObject::HashCode();
  Util::Hash(hash, startPosition);
  Util::Hash(hash, damage);
  Util::Hash(hash, speed);
  Util::Hash(hash, id);
  Util::Hash(hash, isAlive);
  Util::Hash(hash, type);
  return hash;
}

void Projectile::PerformAction() {
  Vector2f v = speed * Util::MakeUnitVector(rotation);

  CollisionTestResult collision
      = gameState.TestCollision(position, position + v, 0, owner);

  if (collision.point != position + v) {
    isAlive = false;

    if (collision.unitHit != nullptr) {
      collision.unitHit->OnAttacked(*this);
    }
  }

  position = collision.point;
}

// Damage is calculated by estimating the expected damage of a shot fired
// at random in a sector centered about the actual trajectory.
float Projectile::CalculateDamage(const GameUnit &unitHit) const {
  float visibleSize = sectorMap.VisibleSize(unitHit.SegmentFromUnit(startPosition));

  // This represents the angular size of the cone of fire.
  float dispersion = Util::Radians(10);
  return damage * std::min(1.f, visibleSize / dispersion);
}

Vector2f Projectile::Origin() const {
  return startPosition;
}


ProjectileModel::ProjectileModel(const Projectile &projectile) {
	position = projectile.Position();
	id = projectile.Id();
  type = projectile.Type();
  rotation = projectile.Rotation();
}

ProjectileModel::ProjectileModel(const ProjectileModel &projectile1,
                                 const ProjectileModel &projectile2,
                                 float weight) {
  type = projectile1.Type();
	float b = weight;
	float a = 1 - weight;
	position = a*projectile1.Position() + b*projectile2.Position();
	id = projectile1.Id();
  rotation = Util::InterpolateAngles(projectile1.Rotation(),
      projectile2.Rotation(), weight);
}