#include "stdafx.h"
#include "Projectile.h"
#include "Util.h"
#include "GameState.h"

int Projectile::serial_number_ = 0;

Projectile::Projectile(GameState& gameState,
                       std::shared_ptr<GameUnit> owner,
                       const Vector2f& position,
                       float direction,
                       float damage,
                       float speed) : gameState(gameState) {
  this->owner = owner;
  name = owner->Attributes().name();
  this->damage = damage;
  this->speed = speed;  
  this->position = position;
  startPosition = position;
	id = serial_number_++;
  isAlive = true;
  rotation_ = direction;
}

void Projectile::PerformAction() {
  Vector2f v = speed * Util::MakeUnitVector(rotation_);

  CollisionTestResult collision
      = gameState.TestCollision(position, position + v, 0, owner);

  if (collision.point != position + v) {
    isAlive = false;

    if (collision.unitHit != nullptr) {
      float effectiveDamage = CalculateDamage(collision.unitHit);
      collision.unitHit->ModifyHealth(-effectiveDamage);
    }
  }

  position = collision.point;
}

// Damage is calculated by estimating the expected damage of a shot fired
// at random in a sector centered about the actual trajectory.
float Projectile::CalculateDamage(std::shared_ptr<GameUnit> unitHit) const {
  float distance = Util::Distance(startPosition,
      unitHit->Position()) + unitHit->Attributes().CollisionRadius();

  // This represents half the angle of the sector.
  float dispersion = Util::Radians(5);

  Vector2f leftVector = Util::MakeUnitVector(Rotation() + dispersion);
  Vector2f leftPoint = startPosition + distance * leftVector;
  Vector2f rightVector = Util::MakeUnitVector(Rotation() - dispersion);
  Vector2f rightPoint = startPosition + distance * rightVector;

  int nRayCasts = 20;
  int nHits = 0;
  for (int i = 0; i < nRayCasts; ++i) {
    float a = float(i) / nRayCasts;
    float b = 1 - a;
    Vector2f interpolatedPoint = a * leftPoint + b * rightPoint;
    CollisionTestResult collision =
      gameState.TestCollision(startPosition, interpolatedPoint, 0, owner);
    if (collision.unitHit == unitHit) {
      ++nHits;
    }
  }

  // We know that the projectile hit the target, but it's possible none of
  // these rays did. Accuracy could be improved somewhat by sweeping over the
  // portion of the unit that is in the dispersion sector, rather than the
  // whole sector.
  // TODO: ignore occlusion by units.
  nHits = std::max(nHits, 1);
  return damage * float(nHits) / nRayCasts;
}


ProjectileModel::ProjectileModel(const Projectile &projectile) {
	position_ = projectile.Position();
	id_ = projectile.Id();
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