#include "stdafx.h"
#include "Projectile.h"
#include "Util.h"
#include "GameState.h"

Projectile::Projectile(GameState &gameState,
                       int id,
                       std::shared_ptr<GameUnit> owner,
                       const Vector2f &position,
                       float direction,
                       float damage,
                       float dispersion,
                       float speed) : gameState(gameState) {
  this->owner = owner;
  type = owner->Attributes().Type();
  sectorMap = owner->SightMap();
  this->damage = damage;
  this->dispersion = dispersion;
  this->speed = speed;
  this->position = position;
  startPosition = position;
	this->id = id;
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

    if (collision.unitHit != nullptr && collision.unitHit->Owner() != owner->Owner()) {
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
  return damage * std::min(1.f, visibleSize / dispersion);
}

Vector2f Projectile::Origin() const {
  return startPosition;
}


ProjectileModel::ProjectileModel(const Projectile &projectile, bool isVisible) {
	position = projectile.Position();
	id = projectile.Id();
  type = projectile.Type();
  rotation = projectile.Rotation();
  this->isVisible = isVisible;
  owner = projectile.Owner()->Owner();
}

ProjectileModel::ProjectileModel(const ProjectileModel &projectile1,
                                 const ProjectileModel &projectile2,
                                 float weight) {
  type = projectile1.Type();
  isVisible = projectile2.isVisible;
  owner = projectile1.owner;
	float b = weight;
	float a = 1 - weight;
	position = a*projectile1.Position() + b*projectile2.Position();
	id = projectile1.Id();
  rotation = Util::InterpolateAngles(projectile1.Rotation(),
      projectile2.Rotation(), weight);
}