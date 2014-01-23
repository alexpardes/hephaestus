#include "stdafx.h"
#include "Pathing.h"

//float LineSegment::Distance(const Vector2f &point) const {
//	float distance;
//	Vector2f line_vector = point1_ - point2_;
//	Vector2f normal_segment = Util::Perpendicular(line_vector);
//	Vector2f point_vector = point - point1_;
//
//	//	Checks if point1 is the closest part of the segment to point.
//	if (Util::Laterality(normal_segment, point_vector) !=
//		Util::Laterality(normal_segment, point2_ - point1_)) {
//		distance = Util::Distance(point1_, point);
//	} else {
//
//		//	Checks if point2 is the closest part of the segment to point.
//		point_vector = point - point2_;
//		if (Util::Laterality(normal_segment, point_vector) !=
//			Util::Laterality(normal_segment, point1_ - point2_)) {
//			distance = Util::Distance(point2_, point);
//		} else {
//
//			//	Gives perpendicular distance from point to the line segment.
//			distance = std::abs(Util::Cross(point_vector, line_vector) /
//					   Util::Length(line_vector));
//		}
//	}
//	return distance;
//}
//
////	Returns the perpendicular distance between the point and the line.
//float Line::Distance(const Vector2f &point) const {
//	float distance;
//	Vector2f point_vector = point - point_;
//	distance = std::abs(Util::Cross(point_vector, direction_) /
//					   Util::Length(direction_));
//	return distance;
//}
//
////	Returns the square of the distance between the point and the line.
//float Line::Distance2(const Vector2f &point) const {
//	float distance;
//	Vector2f point_vector = point - point_;
//	float cross_product = Util::Cross(point_vector, direction_);
//	distance = cross_product * cross_product / Util::Length2(direction_);
//	return distance;
//}
//
////	Divides by zero if lines are parallel.
//Vector2f Line::FindIntersection(const Line &other) const {
//	float ax, ay, bx, by, cx, cy, dx, dy;
//	if (direction_.x != 0) {
//		ax = direction_.x;
//		ay = direction_.y;
//		bx = point_.x;
//		by = point_.y;
//		cx = other.direction_.x;
//		cy = other.direction_.y;
//		dx = other.point_.x;
//		dy = other.point_.y;
//	} else {
//		ax = other.direction_.x;
//		ay = other.direction_.y;
//		bx = other.point_.x;
//		by = other.point_.y;
//		cx = direction_.x;
//		cy = direction_.y;
//		dx = point_.x;
//		dy = point_.y;
//	}
//	float t = (ay * (dx - bx) + ax * (by - dy)) / (ax*cy - ay*cx);
//	float x = cx*t + dx;
//	float y = cy*t + dy;
//	return Vector2f(x, y);
//}
//
//bool LineSegment::Intersects(const Disk &disk) const {
//	return Distance(disk.position()) <= disk.radius();
//}
//
//bool Disk::Intersects(const Disk &disk) const {
//	return Util::Distance(position_, disk.position()) <=
//			radius_ + disk.radius();
//}
//
////	Returns a line tangent to both disks and which passes between the disks.
//Line Pathfinder::FindTangent(const Disk &disk1,
//							 const Disk &disk2,
//							 bool choice) {
//	Vector2f direction = disk2.position() - disk1.position();
//	assert(Util::Length(direction) > disk1.radius() + disk2.radius());
//	float radius_fraction = disk1.radius() / (disk1.radius() + disk2.radius());
//
//	//	Represents the intersection between the tangent lines and the line
//	//	passing through the centers of the disks.
//	Vector2f intersection = disk1.position() + radius_fraction*direction;
//
//	//	The distance from center of disk1 to intersection.
//	float distance = Util::Length(direction) * radius_fraction;
//	float angle = std::asinf(disk1.radius() / distance);
//	if (choice == Pathfinder::kClockwise) angle *= -1;
//	Util::Rotate(direction, angle);
//	return Line(direction, intersection);
//}
//
//CollisionArea::CollisionArea(const Vector2f &start_position,
//							 const Vector2f &end_position,
//							 float radius) :
//		start_position_(start_position),
//		end_position_(end_position),
//		disk_(end_position, radius),
//		radius_(radius) {
//	Vector2f direction =
//			Util::Perpendicular(end_position - start_position);
//	Util::Normalize(direction);
//	direction *= radius;
//	segment1_ = LineSegment(start_position + direction,
//							end_position + direction);
//	segment2_ = LineSegment(start_position - direction,
//							end_position - direction);
//}
//
//
////	Returns the square of the distance that would be traveled by the moving
////	disk before	colliding with the other disk. Returns negative if the static
////	disk does not intersect the CollisionArea.
//float CollisionArea::FindIntersectionDistance(const Disk &disk) const {
//
//	//	The u- and v-coordinates represent the distances from start_position_
//	//	along the direction of motion and the perpendicular direction,
//	//	respectively. The distance to collision is the v-coordinate of the
//	//	first point where the circles would intersect.
//	float distance = -1.f;
//	Line u_axis(start_position_ - end_position_, start_position_);
//	Line v_axis(Util::Perpendicular(start_position_ - end_position_),
//				start_position_);
//
//	//	Checks if the obstacle is "in front of" the starting position.
//	if (Util::Laterality(v_axis.direction(),
//						 disk.position() - start_position_) ==
//		Util::Laterality(v_axis.direction(),
//						 end_position_ - start_position_)) {
//		float disk_u = u_axis.Distance(disk.position());
//		float disk_v = v_axis.Distance(disk.position());
//		float combined_radius = radius_ + disk.radius();
//		float path_length = Util::Distance(start_position_, end_position_);
//		if (combined_radius >= disk_u &&
//			disk_v <= path_length + combined_radius) {
//			float distance_v = disk_v - disk.radius() / (combined_radius) *
//					std::sqrtf(combined_radius*combined_radius - disk_u*disk_u);
//			if (distance_v > path_length) {
//
//				//	Checks if the intersection point is within the moving disk's
//				//	final position.
// 				//	distance_u is the u-coordinate of the intersection point.
//				//float distance_u = radius_*disk_u / combined_radius;
//				//float extra_distance = distance_v - path_length;
//				//if (distance_u*distance_u + extra_distance*extra_distance <=
//				//	radius_*radius_) {
//				if (Util::Distance(end_position_, disk.position()) <= combined_radius) {
//					distance = distance_v;
//				}
//			} else {
//				distance = distance_v;
//			}
//		}
//	}
//	return distance;
//}

const bool Pathfinder::kClockwise = false;
const bool Pathfinder::kCounterClockwise = true;

//Pathfinder::RayCastPathNode *Pathfinder::MakeNode(const GameUnit &unit,
//										   GameUnit *obstacle,
//										   RayCastPathNode *parent,
//										   const Vector2f &destination,
//										   bool choice) const {
//		RayCastPathNode *node = new RayCastPathNode;
//		node->position = FindPathAround(unit,
//									   *obstacle,
//									   parent->position,
//									   destination,
//									   choice);
//		node->parent = parent;
//		node->choice = choice;
//		node->obstacle = obstacle;
//		node->distance_from_root = parent->distance_from_root +
//				Util::Distance(node->position, parent->position);
//		node->min_path_length = parent->distance_from_root +
//				Util::Distance(node->position, destination);
//		return node;
//}

//GameUnit *Pathfinder::FindFirstObstacle(const GameUnit &unit,
//										const Vector2f &start,
//										const Vector2f &end) const {
//	GameUnit *first_obstacle = NULL;
//	float distance_to_obstacle = FLT_MAX;
//	if (end != start) {
//		CollisionArea collision_area(start,
//									 end,
//									 unit.Attributes().collision_radius());
//		for (std::list<GameUnit *>::const_iterator unit_iterator =
//			 game_state_.units().begin();
//			 unit_iterator != game_state_.units().end();
//			 ++unit_iterator) {
//			if (unit != **unit_iterator) {
//				GameUnit &unit2 = **unit_iterator;
//				//	The square of the distance the unit can travel before
//				//	colliding with the obstacle.
//				float collision_distance =
//						collision_area.FindIntersectionDistance(Disk(
//								unit2.position(),
//								unit2.Attributes().collision_radius()));
//				if (collision_distance > 0 &&
//					collision_distance < distance_to_obstacle) {
//					distance_to_obstacle = collision_distance;
//					first_obstacle = *unit_iterator;
//				}
//			}
//		}
//	}
//	return first_obstacle;
//}
//
//Vector2f Pathfinder::FindPathAround(const GameUnit &unit,
//									const GameUnit &obstacle,
//									const Vector2f &start,
//									const Vector2f &destination,
//									bool choice) const {
//	float margin = 0.1f;
//	float obstacle_radius = obstacle.Attributes().collision_radius();
//
//	//	The line the rightmost or leftmost point of the unit follows
//	//	toward the obstacle.
//	Line path1 = FindTangent(Disk(start,
//								  unit.Attributes().collision_radius()),
//							 Disk(obstacle.position(),
//								  obstacle_radius),
//								  choice);
//	Line path2 = FindTangent(Disk(obstacle.position(),
//								  obstacle_radius),
//							 Disk(destination,
//								  unit.Attributes().collision_radius()),
//								  !choice);
//	float angle = Util::FindAngle(path1.direction(), path2.direction());
//	if (choice == kCounterClockwise) angle = 2*M_PI - angle;
//
//	Vector2f intermediate_destination;
//
//	if (angle == 0 || angle == 2*M_PI) {
//		intermediate_destination = obstacle.position() +
//				path1.direction() *
//				(obstacle_radius + unit.Attributes().collision_radius());
//		intermediate_destination = FindPathAround(unit,
//												  obstacle,
//												  start,
//												  intermediate_destination,
//												  choice);
//	} else {
//		//	offset is distance between the center of the disk and the point
//		//	which will touch the obstacle. Must add a margin to avoid
//		//	single point intersection.
//		float offset_direction = (choice == kClockwise) ? -1.f : 1.f;
//		Vector2f offset =
//				unit.Attributes().collision_radius() *
//				path1.GetNormalUnit() * offset_direction;
//		path1.Move(offset);
//		offset =
//				unit.Attributes().collision_radius() *
//				path2.GetNormalUnit() * offset_direction;
//		path2.Move(offset);
//		Vector2f intersection = path1.FindIntersection(path2);
//		offset = intersection - obstacle.position();
//		Util::Normalize(offset);
//		intersection += offset * margin;
//
//		if (angle > M_PI * 2/5) {
//			//	Finds the point on the obstacle circle directly between path1
//			//	and path2, on the side that the unit will travel.
//			Vector2f intersection_direction =
//					intersection - obstacle.position();
//			Util::Normalize(intersection_direction);
//			if (angle > M_PI) intersection_direction *= -1.f;
//			intermediate_destination =
//					obstacle.position() +
//					intersection_direction *
//					(obstacle_radius + unit.Attributes().collision_radius() + margin);
//			intermediate_destination = FindPathAround(unit,
//													  obstacle,
//													  start,
//													  intermediate_destination,
//													  choice);
//		} else {
//			intermediate_destination = intersection;
//		}
//	}
//	return intermediate_destination;
//};
//
//
////	Sets the node's parent to be the earliest node in its ancestry from which
////	it can be reached directly, and updates the distance accordingly.
//void Pathfinder::SimplifyPath(const GameUnit &unit, RayCastPathNode &node) const {
//	std::stack<RayCastPathNode *> ancestor_stack;
//	RayCastPathNode *ancestor = NULL;
//	if (node.parent) ancestor = node.parent->parent;
//	while (ancestor != NULL) {
//		ancestor_stack.push(ancestor);
//		ancestor = ancestor->parent;
//	}
//	while (!ancestor_stack.empty()) {
//		if (!FindFirstObstacle(unit, ancestor_stack.top()->position, node.position)) {
//			node.parent = ancestor_stack.top();
//			node.distance_from_root =
//					Util::Distance(node.position, node.parent->position) +
//					node.parent->distance_from_root;
//			node.min_path_length =
//					Util::Distance(unit.destination(), node.position) +
//					node.distance_from_root;
//			break;
//		}
//		ancestor_stack.pop();
//	}
//}
//
//void Pathfinder::RayCastPathUnit(GameUnit &unit) const {
//	if (unit.destination() == unit.position()) return;
//	std::priority_queue<RayCastPathNode*, std::vector<RayCastPathNode*>, RayCastGreater> queue;
//	std::vector<RayCastPathNode*> node_list;
//	std::set<GameUnit*> obstacle_list;
//	RayCastPathNode *root = new RayCastPathNode;
//	root->position = unit.position();
//	root->distance_from_root = 0;
//	root->min_path_length =
//			Util::Distance(unit.position(), unit.destination());
//	root->parent = NULL;
//	root->choice = kClockwise;
//	node_list.push_back(root);
//	queue.push(root);
//	bool solution_found = false;
//	Vector2f pathing_destination = unit.destination();
//
//	while (!solution_found && !queue.empty()) {
//		RayCastPathNode *node = queue.top();
//		queue.pop();
//		GameUnit *obstacle = NULL;
//		if (node->parent) {
//			obstacle = FindFirstObstacle(unit,
//										 node->parent->position,
//										 node->position);
//		}
//		if (obstacle) {
//			assert(obstacle != node->obstacle);
//			if (!obstacle_list.count(obstacle)) {
//				obstacle_list.insert(obstacle);
//				RayCastPathNode *revision = MakeNode(unit,
//											  obstacle,
//											  node->parent,
//											  unit.destination(),
//											  node->choice);
//				SimplifyPath(unit, *revision);
//				node_list.push_back(revision);
//				queue.push(revision);
//			}
//		} else {
//			obstacle = FindFirstObstacle(unit,
//										 node->position,
//										 unit.destination());
//			if (obstacle) {
//				obstacle_list.insert(obstacle);
//				RayCastPathNode *child1 = MakeNode(unit,
//											obstacle,
//											node,
//											unit.destination(),
//											node->choice);
//				SimplifyPath(unit, *child1);
//				node_list.push_back(child1);
//				queue.push(child1);
//				if (obstacle != node->obstacle) {
//					RayCastPathNode *child2 = MakeNode(unit,
//												obstacle,
//												node,
//												unit.destination(),
//												!node->choice);
//					SimplifyPath(unit, *child2);
//					node_list.push_back(child2);
//					queue.push(child2);
//				}
//			} else {
//				solution_found = true;
//				if (node == root) {
//					pathing_destination = unit.destination();
//				} else {
//					while (node->parent != root) {
//						node = node->parent;
//					}
//					pathing_destination = node->position;
//				}
//			}
//		}
//	}
//	unit.set_pathing_destination(pathing_destination);
//	while (!node_list.empty()) {
//		RayCastPathNode *node = node_list.back();
//		node_list.pop_back();
//		delete node;
//	}
//}

std::stack<Vector2f> Pathfinder::FindAStarPath(const GameUnit &unit,
											  float range) const {
	std::stack<Vector2f> solution_path;
	int **pathing_grid = game_state_.pathing_grid();
	float resolution = game_state_.kPathingResolution;
	int goal_radius = (int) (range / resolution);
	int width = game_state_.pathing_width();
	int height = game_state_.pathing_height();
	bool **visited = new bool*[width];
	for (int i = 0; i < width; ++i) {
		visited[i] = new bool[height];
		for (int j = 0; j < height; ++j) {
			visited[i][j] = false;
		}
	}
	//	The cells bounding the unit form a square with length 2*unit_size + 1.
	int unit_size = (int) (unit.Attributes().collision_radius() /
			game_state_.kPathingResolution + 0.5f);
	int end_x = (int) (unit.destination().x / game_state_.kPathingResolution);
	int end_y = (int) (unit.destination().y / game_state_.kPathingResolution);

	std::priority_queue<PathNode*, std::vector<PathNode*>, Greater> open_list;
	std::vector<PathNode*> node_list;
	int start_x = (int) (unit.position().x / game_state_.kPathingResolution);
	int start_y = (int) (unit.position().y / game_state_.kPathingResolution);

	int x_dist = std::labs(end_x - start_x);
	int y_dist = std::labs(end_y - start_y);
	float f = std::max(x_dist, y_dist) + .4142135624f*std::min(x_dist, y_dist);
	PathNode *root = new PathNode(start_x, start_y, NULL, 0.f, f);
	open_list.push(root);
	node_list.push_back(root);
	bool solution_found = false;
	PathNode *closest_node = root;
	int num_nodes_checked = 0;
	while (!solution_found && !open_list.empty() &&
			++num_nodes_checked < 500) {
		PathNode *node = open_list.top();
		open_list.pop();
		if (!visited[node->x][node->y]) {
			if (node->f - node->distance_from_root <
					closest_node->f - closest_node->distance_from_root) {
				closest_node = node;
			}
			if (node->f - node->distance_from_root <= goal_radius) {
				solution_found = true;
			} else {
				visited[node->x][node->y] = true;
				for (int i = -1; i <= 1; ++i) {
					for (int j = -1; j <= 1; ++j) {
						int x = node->x + i;
						int y = node->y + j;
						if (x - unit_size > 0 && x + unit_size < width &&
								y - unit_size > 0 && y + unit_size < height &&
								!visited[x][y]) {
							bool is_diagonal = i != 0 && j != 0;
							bool position_clear = true;

							//if (pathing_grid[x][y]) {
							//	position_clear = false;
							//} else if (is_diagonal &&
							//		(pathing_grid[x][node->y] ||
							//		pathing_grid[node->x][y])) {
							//	position_clear = false;
							//}

							if (is_diagonal) {
								int x_offset = x + i*unit_size;
								int y_offset = y + j*unit_size;
								if (pathing_grid[x_offset][y_offset]) {
									int z = pathing_grid[x_offset][y_offset];
									position_clear = false;
								} else {
									for (int k = -unit_size;
											k <= unit_size; ++k) {
										if (pathing_grid[x + k][y_offset] ||
											pathing_grid[x_offset][y + k]) {
											position_clear = false;
											break;
										}
									}
								}
							} else {
								for (int k = -unit_size; k <= unit_size; ++k) {
									int new_x = x + i*unit_size + j*k;
									int new_y = y + j*unit_size + i*k;
									if (pathing_grid[new_x][new_y]) {
										position_clear = false;
										break;
									}
								}
							}
							if (position_clear) {
								float distance = node->distance_from_root;
								if (is_diagonal) {
									distance += 1.414213562f;
								} else {
									distance += 1.f;
								}
								int x_dist = std::labs(end_x - x);
								int y_dist = std::labs(end_y - y);
								float f = std::max(x_dist, y_dist) +
										.4142135624f*std::min(x_dist, y_dist) +
										distance;
								PathNode* child =
										new PathNode(x, y, node, distance, f);
								open_list.push(child);
								node_list.push_back(child);
							}
						}
					}
				}
			}
		}
	}
	Vector2f location;
	PathNode *node = closest_node;
	if (node->x == end_x && node->y == end_y)
			solution_path.push(unit.destination());
	while (node != root) {
		location.x = (node->x + .5f) * resolution;
		location.y = (node->y + .5f) * resolution;
		solution_path.push(location);
		node = node->parent;
	}
	while (!node_list.empty()) {
		delete node_list.back();
		node_list.pop_back();
	}
	for (int i = 0; i < width; ++i) {
		delete visited[i];
	}
	delete visited;
	return solution_path;
}

Vector2i Pathfinder::RotateCW(const Vector2i &direction) {
	return Vector2i(direction.y, -direction.x);
}

Vector2i Pathfinder::RotateCCW(const Vector2i &direction) {
	return Vector2i(-direction.y, direction.x);
}

// Should maybe return a path class.
std::stack<Vector2f> Pathfinder::FindPath(const GameUnit &unit, float range)
		const {
	std::stack<Vector2f> solution_path;
	int **pathing_grid = game_state_.pathing_grid();
	float resolution = game_state_.kPathingResolution;
	int goal_radius = (int) (range / resolution - .7071067812f);
	int width = game_state_.pathing_width();
	int height = game_state_.pathing_height();
	bool ***visited = new bool**[width];
	for (int i = 0; i < width; ++i) {
		visited[i] = new bool*[height];
		for (int j = 0; j < height; ++j) {
			visited[i][j] = new bool[4];
			for (int k = 0; k < 4; ++k) {
				visited[i][j][k] = false;
			}
		}
	}
	//	The cells bounding the unit form a square with length 2*unit_size + 1.
	int unit_size = (int) (unit.Attributes().collision_radius() /
			game_state_.kPathingResolution + 0.5f);
	int end_x = (int) (unit.destination().x / game_state_.kPathingResolution);
	int end_y = (int) (unit.destination().y / game_state_.kPathingResolution);

	std::priority_queue<PathNode*, std::vector<PathNode*>, Greater> open_list;
	std::vector<PathNode*> node_list;
	int start_x = (int) (unit.position().x / game_state_.kPathingResolution);
	int start_y = (int) (unit.position().y / game_state_.kPathingResolution);

	int x_dist = std::labs(end_x - start_x);
	int y_dist = std::labs(end_y - start_y);
	float f = std::max(x_dist, y_dist) + .4142135624f*std::min(x_dist, y_dist);
	PathNode *root = new PathNode(start_x, start_y, NULL, 0.f, f);
	open_list.push(root);
	node_list.push_back(root);
	bool solution_found = false;
	PathNode *closest_node = root;
	int num_nodes_checked = 0;
	while (!solution_found && !open_list.empty() &&
			++num_nodes_checked < 500) {
		PathNode *node = open_list.top();
		open_list.pop();
		if (node->hug_direction != Vector2i(0, 0) &&
				IsVisited(Vector2i(node->x, node->y), Vector2i(0, 0),
				node->hug_direction, visited)) {
			continue;
		}
		if (node->f - node->distance_from_root <
				closest_node->f - closest_node->distance_from_root) {
			closest_node = node;
		}
		if (node->f - node->distance_from_root <= goal_radius) {
			solution_found = true;
		} else {
			if (node->hug_direction != Vector2i(0, 0)) {
				MarkVisited(Vector2i(node->x, node->y),
						node->hug_direction, visited);
				if (CanMoveInDirection(Vector2i(node->x, node->y),
						node->hug_direction, unit_size)) {
					PathNode *child = MakeChildNode(node,
							node->hug_direction,
							Vector2i(end_x, end_y));
					open_list.push(child);
					node_list.push_back(child);
				} else {
					Vector2i direction = node->hug_direction;
					Vector2i child_direction = RotateCW(direction);
					Vector2i child_hug = direction;
					for (int i = 0; i < 2; ++i) {
						if (IsMoveOffGrid(Vector2i(node->x, node->y),
								child_direction, unit_size) ||
								IsVisited(Vector2i(node->x, node->y),
								child_direction, child_hug, visited)) {
							break;
						}
						if (CanMoveInDirection(Vector2i(node->x,
								node->y),
								child_direction,
								unit_size)) {
							PathNode *child = MakeChildNode(node,
									child_direction,
									Vector2i(end_x, end_y));
							child->hug_direction = child_hug;
							open_list.push(child);
							node_list.push_back(child);
							break;
						}
						child_direction = RotateCW(child_direction);
						child_hug = RotateCW(child_hug);
					}
					child_direction = RotateCCW(direction);
					child_hug = direction;
					for (int i = 0; i < 2; ++i) {
						if (IsMoveOffGrid(Vector2i(node->x, node->y),
								child_direction, unit_size) ||
								IsVisited(Vector2i(node->x, node->y),
								child_direction, child_hug, visited)) {
							break;
						}
						if (CanMoveInDirection(Vector2i(node->x,
								node->y),
								child_direction,
								unit_size)) {
							PathNode *child = MakeChildNode(node,
									child_direction,
									Vector2i(end_x, end_y));
							child->hug_direction = child_hug;
							open_list.push(child);
							node_list.push_back(child);
							break;
						}
						child_direction = RotateCCW(child_direction);
						child_hug = RotateCCW(child_hug);
					}
				}
			} else {
				Vector2i direction(0, 0);
				if (end_x > node->x) {
					direction.x = 1;
				} else if (end_x < node->x) {
					direction.x = -1;
				}
				if (end_y > node->y) {
					direction.y = 1;
				} else if (end_y < node->y) {
					direction.y = -1;
				}
				if (!IsMoveOffGrid(Vector2i(node->x, node->y), direction,
						unit_size)) {
					if (CanMoveInDirection(Vector2i(node->x, node->y),
							direction, unit_size)) {
						PathNode *child = MakeChildNode(node, direction,
								Vector2i(end_x, end_y));
						open_list.push(child);
						node_list.push_back(child);
					} else {
						bool is_diagonal = direction.x != 0 &&
								direction.y != 0;
						Vector2i child_direction;
						Vector2i child_hug;
						if (is_diagonal) {
							if (direction.x != direction.y) {
								child_direction = Vector2i(0, direction.y);
								child_hug = Vector2i(direction.x, 0);
							} else {
								child_direction = Vector2i(direction.x, 0);
								child_hug = Vector2i(0, direction.y);
							}
						} else {
							child_direction = RotateCW(direction);
							child_hug = direction;
						}
						for (int i = 0; i < 3; ++i) {
							if (IsMoveOffGrid(Vector2i(node->x, node->y),
									child_direction, unit_size) ||
									IsVisited(Vector2i(node->x, node->y),
									child_direction, child_hug, visited)) {
								break;
							}
							if (CanMoveInDirection(Vector2i(node->x,
									node->y),
									child_direction,
									unit_size)) {
								PathNode *child = MakeChildNode(node,
										child_direction,
										Vector2i(end_x, end_y));
								child->hug_direction = child_hug;
								open_list.push(child);
								node_list.push_back(child);
								break;
							}
							child_direction = RotateCW(child_direction);
							child_hug = RotateCW(child_hug);
						}
						if (is_diagonal) {
							if (direction.x == direction.y) {
								child_direction = Vector2i(0, direction.y);
								child_hug = Vector2i(direction.x, 0);
							} else {
								child_direction = Vector2i(direction.x, 0);
								child_hug = Vector2i(0, direction.y);
							}
						} else {
							child_direction = RotateCCW(direction);
							child_hug = direction;
						}
						for (int i = 0; i < 3; ++i) {
							if (IsMoveOffGrid(Vector2i(node->x, node->y),
									child_direction, unit_size) ||
									IsVisited(Vector2i(node->x, node->y),
									child_direction, child_hug, visited)) {
								break;
							}
							if (CanMoveInDirection(Vector2i(node->x,
									node->y),
									child_direction,
									unit_size)) {
								PathNode *child = MakeChildNode(node,
										child_direction,
										Vector2i(end_x, end_y));
								child->hug_direction = child_hug;
								open_list.push(child);
								node_list.push_back(child);
								break;
							}
							child_direction = RotateCCW(child_direction);
							child_hug = RotateCCW(child_hug);
						}
					}
				}
			}
		}
	}
	Vector2f location;
	PathNode *node = closest_node;
	if (node->x == end_x && node->y == end_y)
			solution_path.push(unit.destination());
	while (node != root) {
		location.x = (node->x + .5f) * resolution;
		location.y = (node->y + .5f) * resolution;
		solution_path.push(location);
		node = node->parent;
	}
	while (!node_list.empty()) {
		delete node_list.back();
		node_list.pop_back();
	}
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < 4; ++j) {
			delete visited[i][j];
		}
	}
	delete visited;
	return solution_path;
}

void Pathfinder::MarkVisited(const Vector2i &location,
							 const Vector2i &hug_direction,
							 bool ***visited) {
	visited[location.x][location.y]
			[hug_direction.x + 1 + (hug_direction.y + 1)/2] = true;
}

bool Pathfinder::IsVisited(const Vector2i &location,
						   const Vector2i &direction,
						   const Vector2i &hug_direction,
						   bool ***visited) {
	int x = location.x + direction.x;
	int y = location.y + direction.y;
	return visited[x][y][hug_direction.x + 1 + (hug_direction.y + 1)/2];
}

bool Pathfinder::IsMoveOffGrid(const Vector2i &location,
							   const Vector2i &direction,
							   int unit_size) const {
	int x = location.x + direction.x;
	int y = location.y + direction.y;
	int **pathing_grid = game_state_.pathing_grid();
	int width = game_state_.pathing_width();
	int height = game_state_.pathing_height();
	return x - unit_size < 0 || x + unit_size >= width || y - unit_size < 0 ||
			y + unit_size >= height;
}

bool Pathfinder::CanMoveInDirection(const Vector2i &location,
									const Vector2i &direction,
									int unit_size) const {
	int i = direction.x;
	int j = direction.y;
	int x = location.x + i;
	int y = location.y + j;
	int **pathing_grid = game_state_.pathing_grid();
	bool position_clear = true;
	bool is_diagonal = i != 0 && j != 0;
	if (is_diagonal) {
		int x_offset = x + i*unit_size;
		int y_offset = y + j*unit_size;
		if (pathing_grid[x_offset][y_offset]) {
			position_clear = false;
		} else {
			for (int k = -unit_size;
					k <= unit_size; ++k) {
				if (pathing_grid[x + k][y_offset] ||
					pathing_grid[x_offset][y + k]) {
					position_clear = false;
					break;
				}
			}
		}
	} else {
		for (int k = -unit_size; k <= unit_size; ++k) {
			int new_x = x + i*unit_size + j*k;
			int new_y = y + j*unit_size + i*k;
			if (pathing_grid[new_x][new_y]) {
				position_clear = false;
				break;
			}
		}
	}
	return position_clear;
}

void Pathfinder::PathUnit(GameUnit &unit, float range) const {
	//if (unit.is_stationary()) {
	game_state_.RemoveFromPathingGrid(unit);
	//	unit.set_is_stationary(false);
	//}

	unit.set_path(FindPath(unit, range));
	game_state_.AddToPathingGrid(unit);
}