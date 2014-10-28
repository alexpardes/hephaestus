#include "stdafx.h"
#include "Commands.h"
#include "GameInterface.h"

const float GameInterface::kScrollSpeed = 500.f;

Command *GameInterface::ProcessEvent(const sf::Event &event,
                                     const sf::RenderWindow &window) {
	Vector2f location;
	Command *command = NULL;
	switch (event.type) {
    case sf::Event::Resized:
      Resize();
		case sf::Event::MouseButtonPressed: {
			Vector2i mouseLocation(event.mouseButton.x, event.mouseButton.y);
      location = GetGameLocation(mouseLocation);
			switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					switch (cursor_action_) {
						case kSelect:
							selection_corner1_ = location;
							selection_corner2_ = location;
							is_selecting_ = true;
							break;
						case kAttack:
							const UnitModel *unit = GetUnit(location);
							if (unit) {
								command = new AttackCommand(unit->Id());
							} else {
								command = new AttackMoveCommand(location);
							}
							cursor_action_ = kSelect;
							break;
					}
					break;
				case sf::Mouse::Right:
					if (cursor_action_ == kSelect) {
						const UnitModel *unit = GetUnit(location);
						if (unit && unit->Owner() != player_) {
							command = new AttackCommand(unit->Id());
						} else {
							command = new MoveCommand(location);
						}
					} else {
						cursor_action_ = kSelect;
					}
					break;
			}
			break;
    }
    case sf::Event::MouseLeft:
      MouseScroll();
      break;
		case sf::Event::MouseMoved: {
      MouseScroll();
			if (is_selecting_) {
        Vector2i mouseLocation = Vector2i(event.mouseMove.x,
            event.mouseMove.y);
        location = GetGameLocation(mouseLocation);
				selection_corner2_ = location;
			}
			break;
    }
		case sf::Event::MouseButtonReleased:
			switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					if (is_selecting_) {
						selected_unit_ids_.clear();
            std::vector<UnitId> ids = GetUnitIds(selection_corner1_,
								selection_corner2_);
            for (UnitId id : ids) {
              selected_unit_ids_.push_back(id);
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
					keyboardHScroll = -1;
					break;
				case sf::Keyboard::Key::Up:
					keyboardVScroll = -1;
					break;
				case sf::Keyboard::Key::Right:
					keyboardHScroll = 1;
					break;
				case sf::Keyboard::Key::Down:
					keyboardVScroll = 1;
					break;
			}
			break;
		case sf::Event::KeyReleased:
			switch (event.key.code) {
        // TODO: are these ifs necessary?
				case sf::Keyboard::Key::Left:
					if (keyboardHScroll == -1)
							keyboardHScroll = 0;
					break;
				case sf::Keyboard::Key::Up:
					if (keyboardVScroll == -1)
							keyboardVScroll = 0;
					break;
				case sf::Keyboard::Key::Right:
					if (keyboardHScroll == 1)
							keyboardHScroll = 0;
					break;
				case sf::Keyboard::Key::Down:
					if (keyboardVScroll == 1)
							keyboardVScroll = 0;
					break;
			}
			break;
	}
	return command;
}

void GameInterface::Resize() {
  int screenHeight = window.getDefaultView().getSize().y;

  float graphicSize = 0.32 * screenHeight;
  interface_graphic_ = sf::RectangleShape(Vector2f(graphicSize,
    graphicSize));
  interface_graphic_.setPosition(Vector2f(0,
    screenHeight - graphicSize));
  interface_graphic_.setFillColor(sf::Color(0, 0, 0, 255));

  SetMapSize(mapSize);
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
  int horizontalDirection = Util::Sign(mouseHScroll + keyboardHScroll);
  int verticalDirection = Util::Sign(mouseVScroll + keyboardVScroll);
	float horizontal_move = horizontalDirection * kScrollSpeed * time_step;
	float vertical_move = verticalDirection * kScrollSpeed * time_step;

  mainView.move(horizontal_move, vertical_move);
  ConstrainView();
}

void GameInterface::MouseScroll() {
  int x = sf::Mouse::getPosition(window).x;
  int y = sf::Mouse::getPosition(window).y;

  int horizontalScrollMargin = 5;
  int verticalScrollMargin = 5;

  int screenWidth = window.getSize().x;
  int screenHeight = window.getSize().y;

  if (x < horizontalScrollMargin) {
    mouseHScroll = -1;
  } else if (screenWidth - x < horizontalScrollMargin) {
    mouseHScroll = 1;
  } else {
    mouseHScroll = 0;
  }
  if (y < verticalScrollMargin) {
    mouseVScroll = -1;
  } else if (screenHeight - y < verticalScrollMargin) {
    mouseVScroll = 1;
  } else {
    mouseVScroll = 0;
  }
}

void GameInterface::ConstrainView() {
  float minX = window.getSize().x / 2;
  float minY = window.getSize().y /2;
  float maxX = mapSize.x - window.getSize().x / 2;
  float maxY = mapSize.y - window.getSize().y / 2;

  float x = mainView.getCenter().x;
  float y = mainView.getCenter().y;
  if (x <= minX) {
    x = minX;
  } else if (x > maxX) {
    x = maxX;
  }
  if (y <= minY) {
    y = minY;
  } else if (y > maxY) {
    y = maxY;
  }
  mainView.setCenter(x, y);
}

sf::Drawable *GameInterface::GetSelectionBoxGraphic() const {
	sf::RectangleShape *selection_box =
			new sf::RectangleShape(selection_corner2_ - selection_corner1_);
	selection_box->setPosition(selection_corner1_);
	selection_box->setFillColor(sf::Color(100, 100, 255, 100));
	selection_box->setOutlineColor(sf::Color(0, 0, 255, 200));
	selection_box->setOutlineThickness(1.f);

	return selection_box;
}

const sf::Drawable *GameInterface::GetInterfaceGrahic() const {
	return &interface_graphic_;
}

const UnitModel *GameInterface::GetUnit(const Vector2f &location) const {
	const UnitModel *unit = NULL;
	std::vector<const UnitModel *> units = game_scene_->
			GetUnitsInRectangle(location, location);
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
	for (const UnitModel * unit : units) {
		if (unit->Owner() == player_) {
			ids.push_back(unit->Id());
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
	for (size_t i = 0; i < dead_unit_ids.size(); ++i) {
		selected_unit_ids_.erase(dead_unit_ids[i]);
	}
}