#include "stdafx.h"
#include "Commands.h"
#include "GameInterface.h"

const float GameInterface::kScrollSpeed = 800.f;
bool GameInterface::hasRegisteredCommands = false;

void GameInterface::RegisterCommands() {
  if (!hasRegisteredCommands) {
    SelectCommand::Register();
    MoveCommand::Register();
    AttackCommand::Register();
    AttackMoveCommand::Register();
    StopCommand::Register();
    TargetGroundCommand::Register();
    hasRegisteredCommands = true;
  }
}

void GameInterface::Reset() {
  keyboardHScroll = 0;
  keyboardVScroll = 0;
  mouseHScroll = 0;
  mouseVScroll = 0;
  isSelecting = false;
  cursorAction = kSelect;
  gameScene = nullptr;
  selectedUnitIds.clear();
}

GameInterface::GameInterface(sf::RenderWindow &window) : window(window) {
  Reset();
  mainView.setSize(Vector2f(1920, 1080));
  RegisterCommands();
}

std::shared_ptr<Command> GameInterface::ProcessEvent(const sf::Event &event,
                                                     const sf::RenderWindow &window) {

  if (!gameScene)
    return nullptr;

	Vector2f location;
	std::shared_ptr<Command> command = nullptr;
	switch (event.type) {
    case sf::Event::Resized:
      Resize();
		case sf::Event::MouseButtonPressed: {
			Vector2i mouseLocation(event.mouseButton.x, event.mouseButton.y);
      location = GetGameLocation(mouseLocation);
			switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					switch (cursorAction) {
						case kSelect:
							selectionCorner1 = location;
							selectionCorner2 = location;
							isSelecting = true;
							break;
						case kAttack: {
							const UnitModel *unit = GetUnit(location);
							if (unit) {
								command = std::make_shared<AttackCommand>(unit->Id());
							} else {
								command = std::make_shared<AttackMoveCommand>(location);
							}
							cursorAction = kSelect;
							break;
            }
            case kTarget:
              command = std::make_shared<TargetGroundCommand>(location);
              cursorAction = kSelect;
              break;
					}
					break;
				case sf::Mouse::Right:
					if (cursorAction == kSelect) {
						const UnitModel *unit = GetUnit(location);
						if (unit && unit->Owner() != player) {
							command = std::make_shared<AttackCommand>(unit->Id());
						} else {
							command = std::make_shared<MoveCommand>(location);
						}
					} else {
						cursorAction = kSelect;
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
			if (isSelecting) {
        Vector2i mouseLocation = Vector2i(event.mouseMove.x,
            event.mouseMove.y);
        location = GetGameLocation(mouseLocation);
				selectionCorner2 = location;
			}
			break;
    }
		case sf::Event::MouseButtonReleased:
			switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					if (isSelecting) {
						selectedUnitIds.clear();
            std::vector<UnitId> ids = GetUnitIds(selectionCorner1,
								selectionCorner2);
            bool selectSingle = selectionCorner1 == selectionCorner2;
            if (selectSingle)
              ids.resize(1);

            for (UnitId id : ids) {
              selectedUnitIds.push_back(id);
            }
						command = std::make_shared<SelectCommand>(ids);
						isSelecting = false;
					}
					break;
			}
			break;
		case sf::Event::KeyPressed:
			switch (event.key.code) {
				case sf::Keyboard::Key::A:
					cursorAction = kAttack;
					break;
        case sf::Keyboard::Key::S:
          command = std::make_shared<StopCommand>();
          break;
        case sf::Keyboard::Key::D:
          cursorAction = kTarget;
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
  float screenHeight = window.getDefaultView().getSize().y;
  float graphicSize = 0.32f * screenHeight;
  interfaceGraphic = sf::RectangleShape(Vector2f(graphicSize,
    graphicSize));
  interfaceGraphic.setPosition(Vector2f(0,
    screenHeight - graphicSize));
  interfaceGraphic.setFillColor(sf::Color(0, 0, 0, 255));

  SetMapSize(mapSize);
}

const std::list<UnitModel *> GameInterface::GetSelectedUnits() const {
	std::list<UnitModel *> units;
	for (std::list<UnitId>::const_iterator id =
			selectedUnitIds.begin(); id != selectedUnitIds.end(); ++id) {
		units.push_back(gameScene->GetUnit(*id));
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
  float minX = mainView.getSize().x / 2;
  float minY = mainView.getSize().y /2;
  float maxX = mapSize.x - mainView.getSize().x / 2;
  float maxY = mapSize.y - mainView.getSize().y / 2;

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
			new sf::RectangleShape(selectionCorner2 - selectionCorner1);
	selection_box->setPosition(selectionCorner1);
	selection_box->setFillColor(sf::Color(100, 100, 255, 100));
	selection_box->setOutlineColor(sf::Color(0, 0, 255, 200));
	selection_box->setOutlineThickness(1.f);

	return selection_box;
}

const sf::Drawable *GameInterface::GetInterfaceGrahic() const {
	return &interfaceGraphic;
}

const UnitModel *GameInterface::GetUnit(const Vector2f &location) const {
	const UnitModel *unit = nullptr;
	std::vector<const UnitModel *> units = gameScene->
			GetUnitsInRectangle(location, location);
	if (units.size()) {
		unit = units.front();
	}
	return unit;
}

const std::vector<UnitId> GameInterface::GetUnitIds(
		const Vector2f &location1, const Vector2f &location2) const {
	std::vector<UnitId> ids;
	const std::vector<const UnitModel *> units = gameScene->
			GetUnitsInRectangle(location1, location2);
	for (const UnitModel * unit : units) {
		if (unit->Owner() == player) {
			ids.push_back(unit->Id());
		}
	}
	return ids;
}

void GameInterface::DeselectDeadUnits() {
	std::vector<std::list<UnitId>::const_iterator> dead_unit_ids;
	for (std::list<UnitId>::const_iterator unit_id = selectedUnitIds.begin();
			unit_id != selectedUnitIds.end(); ++unit_id) {
		if (!gameScene->GetUnit(*unit_id)) {
			dead_unit_ids.push_back(unit_id);
		}
	}
	for (size_t i = 0; i < dead_unit_ids.size(); ++i) {
		selectedUnitIds.erase(dead_unit_ids[i]);
	}
}