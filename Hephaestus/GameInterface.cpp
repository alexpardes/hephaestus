#include "stdafx.h"
#include "Commands.h"
#include "GameInterface.h"

const float GameInterface::kScrollSpeed = 500.f;

Command *GameInterface::ProcessEvent(const sf::Event &event) {
	Vector2i location;
	Command *command = NULL;
	switch (event.type) {
		case sf::Event::MouseButtonPressed:
			location = GetGameLocation(Vector2i(event.mouseButton.x,
					event.mouseButton.y));
			switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					switch (cursor_action_) {
						case kSelect:
							selection_corner1_ = Util::GetVector2f(location);
							selection_corner2_ = Util::GetVector2f(location);
							is_selecting_ = true;
							break;
						case kAttack:
							const UnitModel *unit = GetUnit(location);
							if (unit) {
								//command = new AttackCommand(unit->id());
							} else {
								//command = new AttackMoveCommand(location);
							}
							cursor_action_ = kSelect;
							break;
					}
					break;
				case sf::Mouse::Right:
					if (cursor_action_ == kSelect) {
						const UnitModel *unit = GetUnit(location);
						if (unit && unit->owner() != player_) {
							//command = new AttackCommand(unit->id());
						} else {
							command = new MoveCommand(Util::GetVector2f(location));
						}
					} else {
						cursor_action_ = kSelect;
					}
					break;
			}
			break;
		case sf::Event::MouseMoved:
			if (is_selecting_) {
				location = GetGameLocation(Vector2i(event.mouseMove.x,
						event.mouseMove.y));
				selection_corner2_ = Util::GetVector2f(location);
			}
			break;
		case sf::Event::MouseButtonReleased:
			switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					if (is_selecting_) {
						selected_unit_ids_.clear();
            std::vector<UnitId> ids = GetUnitIds(selection_corner1_,
								selection_corner2_);
            for (size_t i = 0; i < ids.size(); ++i) {
              selected_unit_ids_.push_back(ids[i]);
            }
						command = new SelectCommand(ids);
						is_selecting_ = false;
					}
					break;
			}
			break;
		case sf::Event::KeyPressed:
			switch (event.key.code) {
				case sf::Keyboard::Key::A:
					cursor_action_ = kAttack;
					break;
				case sf::Keyboard::Key::Left:
					horizontal_scroll_ = -1;
					break;
				case sf::Keyboard::Key::Up:
					vertical_scroll_ = -1;
					break;
				case sf::Keyboard::Key::Right:
					horizontal_scroll_ = 1;
					break;
				case sf::Keyboard::Key::Down:
					vertical_scroll_ = 1;
					break;
			}
			break;
		case sf::Event::KeyReleased:
			switch (event.key.code) {
				case sf::Keyboard::Key::Left:
					if (horizontal_scroll_ == -1)
							horizontal_scroll_ = 0;
					break;
				case sf::Keyboard::Key::Up:
					if (vertical_scroll_ == -1)
							vertical_scroll_ = 0;
					break;
				case sf::Keyboard::Key::Right:
					if (horizontal_scroll_ == 1)
							horizontal_scroll_ = 0;
					break;
				case sf::Keyboard::Key::Down:
					if (vertical_scroll_ == 1)
							vertical_scroll_ = 0;
					break;
			}
			break;
	}
	return command;
}

const std::list<UnitModel *> GameInterface::GetSelectedUnits() const {
	std::list<UnitModel *> units;
	for (std::list<UnitId>::const_iterator id =
			selected_unit_ids_.begin(); id != selected_unit_ids_.end(); ++id) {
		units.push_back(game_scene_->GetUnit(*id));
	}
	return units;
}

void GameInterface::MoveScreen(float time_step) {
	float horizontal_move = horizontal_scroll_ * kScrollSpeed * time_step;
	float vertical_move = vertical_scroll_ * kScrollSpeed * time_step;

	//	The last term is to account for the UI size.
	Vector2f bottom_right = map_size_ - screen_size_ + Vector2f(0.f, 200.f);
	screen_position_.x += horizontal_move;
	if (screen_position_.x < 0.f) {
		screen_position_.x = 0.f;
	} else if (screen_position_.x > bottom_right.x) {
		screen_position_.x = bottom_right.x;
	}
	screen_position_.y += vertical_move;
	if (screen_position_.y < 0.f) {
		screen_position_.y = 0.f;
	} else if (screen_position_.y > bottom_right.y) {
		screen_position_.y = bottom_right.y;
	}
	if (is_selecting_) {
		selection_corner2_.x += horizontal_move;
		if (selection_corner2_.x < 0.f) {
			selection_corner2_.x = 0.f;
		} else if (selection_corner2_.x > map_size_.x) {
			selection_corner2_.x = map_size_.x;
		}
		selection_corner2_.y += vertical_move;
		if (selection_corner2_.y < 0.f) {
			selection_corner2_.y = 0.f;
		} else if (selection_corner2_.y > map_size_.y) {
			selection_corner2_.y = map_size_.y;
		}
		if (selection_corner2_.y < 0) selection_corner2_.y = 0;
	}
}

sf::Drawable *GameInterface::GetSelectionBoxGraphic() const {
	sf::RectangleShape *selection_box =
			new sf::RectangleShape(selection_corner2_ - selection_corner1_);
	selection_box->setPosition(selection_corner1_ - screen_position_);
	selection_box->setFillColor(sf::Color(100, 100, 255, 100));
	selection_box->setOutlineColor(sf::Color(0, 0, 255, 200));
	selection_box->setOutlineThickness(1.f);

	return selection_box;
}

const sf::Drawable *GameInterface::GetInterfaceGrahic() const {
	return &interface_graphic_;
}

const UnitModel *GameInterface::GetUnit(const Vector2i &location) const {
	const UnitModel *unit = NULL;
	Vector2f location_f = Util::GetVector2f(location);
	std::vector<const UnitModel *> units = game_scene_->
			GetUnitsInRectangle(location_f, location_f);
	if (units.size()) {
		unit = units.front();
	}
	return unit;
}

const std::vector<UnitId> GameInterface::GetUnitIds(
		const Vector2f &location1, const Vector2f &location2) const {
	std::vector<UnitId> ids;
	const std::vector<const UnitModel *> units = game_scene_->
			GetUnitsInRectangle(location1, location2);
	for (std::vector<const UnitModel *>::const_iterator unit = units.begin();
			unit != units.end(); ++unit) {
		if ((**unit).owner() == player_) {
			ids.push_back((**unit).id());
		}
	}
	return ids;
}

void GameInterface::DeselectDeadUnits() {
	std::vector<std::list<UnitId>::const_iterator> dead_unit_ids;
	for (std::list<UnitId>::const_iterator unit_id = selected_unit_ids_.begin();
			unit_id != selected_unit_ids_.end(); ++unit_id) {
		if (!game_scene_->GetUnit(*unit_id)) {
			dead_unit_ids.push_back(unit_id);
		}
	}
	for (int i = 0; i < dead_unit_ids.size(); ++i) {
		selected_unit_ids_.erase(dead_unit_ids[i]);
	}
}