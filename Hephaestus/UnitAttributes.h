#pragma once

#include <string>

class UnitAttributes {
public:
  UnitAttributes(const std::string &name, float collision_radius,
    float selection_radius, float speed,
    float attack_damage,
    float attack_speed, float attack_range,
    float max_health) : name_(name), speed_(speed),
    collision_radius_(collision_radius),
    selection_radius_(selection_radius),
    attack_damage_(attack_damage), attack_speed_(attack_speed),
    attack_range_(attack_range), max_health_(max_health) { }
  UnitAttributes() { }

  const std::string &name() const {return name_;}

  float speed() const {return speed_;}
  float CollisionRadius() const {return collision_radius_;}
  float selection_radius()  const {return selection_radius_;}
  float attack_damage() const {return attack_damage_;}
  float attack_speed() const {return attack_speed_;}
  float attack_range() const {return attack_range_;}
  float max_health() const {return max_health_;}

  void set_speed(float speed) {speed_ = speed;}
  void set_collision_radius(float collision_radius) {collision_radius_ = collision_radius;}
  void set_selection_radius(float selection_radius) {selection_radius_ = selection_radius;}
  void set_attack_damage(float attack_damage) {attack_damage_ = attack_damage;}
  void set_attack_speed(float attack_speed) {attack_speed_ = attack_speed;}
  void set_attack_range(float attack_range) {attack_range_ = attack_range;}
  void set_max_health(float max_health) {max_health_ = max_health;}

private:
  std::string name_;
  float speed_, collision_radius_, selection_radius_,	attack_damage_,
    attack_speed_, attack_range_, max_health_;
};