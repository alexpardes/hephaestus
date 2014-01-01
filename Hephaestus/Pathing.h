#ifndef PATHING_
#define PATHING_

#include "GameState.h"
#include "Util.h"

class Line;
class Disk;
class LineSegment;
class CollisionArea;
class Pathfinder;

class Disk {
	public:
		Disk(Vector2f position, float radius):
				position_(position), radius_(radius) { }
		float radius() const {return radius_;}
		Vector2f position() const {return position_;}
		void set_radius(float radius) {radius_ = radius;}
		void set_position(const Vector2f &position) {position_ = position;}
		bool Disk::Intersects(const Disk &disk) const;
	
	private:
		Vector2f position_;
		float radius_;
};

class Line {
	public:
		Line(const Vector2f &direction, const Vector2f &point) :
				direction_(direction), point_(point) {
			Util::Normalize(direction_);
		}
		float Distance(const Vector2f &point) const;
		float Distance2(const Vector2f &point) const;
		Vector2f FindIntersection(const Line &other) const;
		Vector2f direction() const {return direction_;}
		Vector2f GetNormalUnit() const {
			return Util::Perpendicular(direction_);
		}
		void Move(const Vector2f &vector) {point_ += vector;}
	private:
		Vector2f direction_, point_;
};

class LineSegment {
	public:
		LineSegment() : point1_(Vector2f(0.f, 0.f)), point2_ (Vector2f(0.f, 0.f)) { }
		LineSegment(const Vector2f &start, const Vector2f &end) :
				point1_(start), point2_(end) { }
		Vector2f point1() const {return point1_;}
		Vector2f point2() const {return point2_;}
		void set_point1(const Vector2f &point) {point1_ = point;}
		void set_point2(const Vector2f &point) {point2_ = point;}
		float Distance(const Vector2f &point) const;
		float DistanceFromLine(const Vector2f &point) const;
		bool Intersects(const Disk &disk) const;

	private:
		Vector2f point1_, point2_;
};

//	Represents the surface traversed by a disk moving along a line from
//	start_position to end_position.
class CollisionArea {
	public:
		CollisionArea(const Vector2f &start_position,
					  const Vector2f &end_position,
					  float radius);

		float FindIntersectionDistance (const Disk &disk) const;
	private:
		Vector2f start_position_, end_position_;
		LineSegment segment1_, segment2_;
		Disk disk_;
		float radius_;
};

class Pathfinder {
	public:
		const static bool kCounterClockwise;
		const static bool kClockwise;
		Pathfinder(GameState &game_state) : game_state_(game_state) { }
		static Line FindTangent(const Disk &disk1,
						 const Disk &disk2,
						 bool choice);
		void RayCastPathUnit(GameUnit &unit) const;
		void PathUnit(GameUnit &unit, float range) const;
		GameUnit *FindFirstObstacle(const GameUnit &unit,
									const Vector2f &start,
									const Vector2f &end) const;
		Vector2f FindPathAround(const GameUnit &unit,
								const GameUnit &obstacle,
								const Vector2f &start,
								const Vector2f &destination,
								bool choice) const;

	private:
		static Vector2i RotateCW(const Vector2i &direction);
		static Vector2i RotateCCW(const Vector2i &direction);
		struct PathNode {
			int x, y;
			PathNode *parent;
			float distance_from_root;
			float f;
			Vector2i hug_direction;
			PathNode(int x, int y, PathNode *parent, float distance, float f) :
					x(x),
					y(y),
					parent(parent),
					distance_from_root(distance),
					f(f),
					hug_direction(0, 0) { }
		};

		PathNode *MakeChildNode (PathNode *parent,
								 const Vector2i &direction,
								 const Vector2i &destination) const {
			float distance = parent->distance_from_root;
			bool is_diagonal = direction.x != 0 && direction.y != 0;
			if (is_diagonal) {
				distance += 1.414213562f;
			} else {
				distance += 1.f;
			}
			int x_dist = std::labs(destination.x - parent->x);
			int y_dist = std::labs(destination.y - parent->y);
			float f = std::max(x_dist, y_dist) +
					.4142135624f*std::min(x_dist, y_dist) +
					distance;
			return new PathNode(parent->x + direction.x, parent->y + direction.y, parent, distance, f);
		}
					
		struct RayCastPathNode {
			friend class Pathfinder;
			private:
				Vector2f position;
				RayCastPathNode *parent;
				GameUnit *obstacle;
				bool choice;
				float distance_from_root;
				float min_path_length;
		};

		static void MarkVisited(const Vector2i &location,
								const Vector2i &hug_direction,
								bool ***visited);
		static bool IsVisited(const Vector2i &location,
							  const Vector2i &direction,
							  const Vector2i &hug_direction,
							  bool ***visited);
		bool IsMoveOffGrid(const Vector2i &location,
									  const Vector2i &direction,
									  int unit_size) const;
		bool CanMoveInDirection(const Vector2i &location,
								const Vector2i &direction,
								int unit_size) const;
		std::stack<Vector2f> FindPath(const GameUnit &unit, float range) const;
		std::stack<Vector2f> FindAStarPath(const GameUnit &unit,
										   float range) const;
		RayCastPathNode *MakeNode(const GameUnit &unit,
						   GameUnit *obstacle,
						   RayCastPathNode *parent,
						   const Vector2f &destination,
						   bool choice) const;
		void SimplifyPath(const GameUnit &unit, RayCastPathNode &node) const;
		struct Greater {
			bool operator()(PathNode *a, PathNode *b) {
				bool result;
				if (a->f == b->f) {
					result = a->distance_from_root < b->distance_from_root;
				} else {
					result = a->f > b->f;
				}
				return result;
			}
		};
		struct RayCastGreater {
			bool operator()(RayCastPathNode *a, RayCastPathNode *b) {
				return a->min_path_length > b->min_path_length;
			}
		};
		GameState &game_state_;
};

#endif