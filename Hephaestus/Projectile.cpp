#include "stdafx.h"
#include "Projectile.h"
#include "Util.h"

//const float Projectile::kSpeed = .75f;
int Projectile::serial_number_ = 0;

Projectile::Projectile(const std::string &name,
                       const Vector2f &position,
                       std::shared_ptr<GameUnit> target,
                       float damage,
                       float speed) {
  this->name = name;
  target_ = target;
  damage_ = damage;
  this->speed = speed;  
  position_ = position;  
	id_ = serial_number_++;
  isAlive = true;

  rotation_ = Util::FindAngle(target->Position() - position);
}

void Projectile::PerformAction() {
  Vector2f v = target_->Position() - position_;
  Util::Limit(v, speed);
  position_ += v;
  rotation_ = Util::FindAngle(v);
  if (Util::Distance(target_->Position(), position_) == 0.f) {
    target_->modify_health(-damage_);
    isAlive = false;
  }
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