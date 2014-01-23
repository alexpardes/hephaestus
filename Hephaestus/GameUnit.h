#ifndef GAMEUNIT_
#define GAMEUNIT_

#include "Util.h"
#include <list>

class UnitAction;
class UnitAbility;

typedef short UnitId;

enum PlayerNumber { kPlayer1, kPlayer2 };

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
		float collision_radius() const {return collision_radius_;}
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

class GameUnit {
	public:
		GameUnit(int id, const UnitAttributes &attributes, PlayerNumber owner,
				const Vector2f &position, float rotation);

		void set_destination(const Vector2f &destination) {
			destination_ = destination;
		}

		void set_pathing_destination(const Vector2f &pathing_destination) {
			pathing_destination_ = pathing_destination;
		}

		void SetPosition(const Vector2f &position) {
			position_ = position;
		}

		void SetRotation(float rotation) {
			rotation_ = rotation;
		}

		void set_velocity(Vector2f velocity) {
			velocity_ = velocity;
		}

		void set_path(std::stack<Vector2f> path) {
			path_ = path;
		}

		void pop_path() {
			if (!path_.empty()) {
				path_.pop();
			}
			if (path_.empty()) {
				destination_ = position_;
			}
		}

		void ClearPath() {
			while (!path_.empty()) path_.pop();
		}

		void set_is_stationary(bool is_stationary) {
			is_stationary_ = is_stationary;
		}

		void set_unit_grid_x(int x) {
			unit_grid_x_ = x;
		}

		void set_unit_grid_y(int y) {
			unit_grid_y_ = y;
		}

		//Returns direction in degrees to pathing destination.
		float GetHeading() const;
		int unit_grid_x() const {return unit_grid_x_;}
		int unit_grid_y() const {return unit_grid_y_;}
		Vector2f position() const {return position_;}
		Vector2f destination() const {return destination_;}
		Vector2f pathing_destination() const {
			if (path_.empty()) {
				return position();
			} else {
				return path_.top();
			}
		}
		Vector2f velocity() const {return velocity_;}
		float Rotation() const {return rotation_;}

		static std::vector<Vector2i> pathing_offsets(int x, int y) {
			return pathing_offsets_[3*x + y + 4];
		}

		void set_target(GameUnit *target) {target_ = target;}
		GameUnit *target() const {return target_;}

		void modify_health(float health) {
			current_health_ += health;
			if (current_health_ < 0) current_health_ = 0;
			if (current_health_ > attributes_.max_health()) {
				current_health_ = attributes_.max_health();
			}
		}

		float current_health() const {return current_health_;}

		void ResetReload();
		void ReduceReloadTime(float time) {
			reload_time_ -= time;
			if (reload_time_ < 0) reload_time_ = 0;
		}
		bool IsLoaded() const {return reload_time_ == 0;}

		void SetAction(UnitAction *action);
    void PerformAction();

		void Kill() {is_alive_ = false;}
		bool IsAlive() const {return is_alive_;}
		bool IsStationary() const {return is_stationary_;}
		bool operator==(const GameUnit &other) const {return this == &other;}
		bool operator!=(const GameUnit &other) const {return this != &other;}
		int Id() const {return id_;}
		PlayerNumber Owner() const {return owner_;}
		const UnitAttributes Attributes() const {return attributes_;}

    void SetAbility(UnitAbility *ability) { this->ability = ability; }

	private:
		Vector2f position_, destination_, pathing_destination_, velocity_;
		int unit_grid_x_, unit_grid_y_;
		std::stack<Vector2f> path_;
		GameUnit *target_;
		float reload_time_;
		float rotation_;
		bool is_alive_;
		bool is_stationary_;
		static std::vector<Vector2i>* pathing_offsets_;
		float current_health_;
		int id_;
		UnitAttributes attributes_;
		PlayerNumber owner_;
    UnitAction *action;
    UnitAbility *ability;
};
typedef std::list<GameUnit*> UnitList;

class UnitModel {
	public:
		explicit UnitModel(const GameUnit &unit);
		UnitModel(const UnitModel &unit1, const UnitModel &unit2,
				float weight);
		float current_health() const {return current_health_;}
		float radius() const {return radius_;}
		Vector2f position() const {return position_;}
		void set_position(const Vector2f &position) {position_ = position;}
		void set_rotation(float rotation) {rotation_ = rotation;}
		float rotation() const {return rotation_;}
		PlayerNumber owner() const {return owner_;}
		const std::string &name() const {return name_;}
		int id() const {return id_;}

	private:
		Vector2f position_;
		float rotation_;
		float current_health_;
		float radius_;
		int id_;
		PlayerNumber owner_;
		std::string name_;
};

#endif