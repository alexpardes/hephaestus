#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "Command.h"
#include "GameState.h"
#include "GameUnit.h"
#include "NetworkManager.h"

class GameInterface {
	public:
		GameInterface(sf::RenderWindow& window) : window(window) {
      keyboardHScroll = 0;
      keyboardVScroll = 0;
			mouseHScroll = 0;
			mouseVScroll = 0;
			is_selecting_ = false;
			cursor_action_ = kSelect;
			game_scene_ = NULL;
      mainView.setCenter(Vector2f(960, 540));
      mainView.setSize(Vector2f(1920, 1080));
		}

    void Resize();

    const sf::View &MainView() const { return mainView; }
    const sf::View &MinimapView() const { return minimapView; }

		Command *ProcessEvent(const sf::Event &event, const sf::RenderWindow &window);
		void MoveScreen(float time_step);

		CommandTurn TakeCommands() {
			CommandTurn old_queue = command_queue_;
			command_queue_ = CommandTurn();
			return old_queue;
		}

    void SetMapSize(const Vector2f &mapSize) {
      int screenWidth = window.getSize().x;
      int screenHeight = window.getSize().y;

      map_size_ = mapSize;
      minimapView.setCenter(map_size_ / 2.f);
      minimapView.setSize(map_size_);
      float mapAspectRatio = map_size_.x / map_size_.y;
      float screenAspectRatio = float(screenWidth) / screenHeight;
      minimapView.setViewport(sf::FloatRect(0.01f / screenAspectRatio, 0.69f,
          0.3f * mapAspectRatio / screenAspectRatio, 0.3f));
    }

    void SetPlayer(PlayerNumber player) {
      player_ = player;
    }

    PlayerNumber Player() const { return player_; }

		//Vector2f screen_position() const {return screen_position_;}
		//void set_screen_position(Vector2f location) {
		//	screen_position_ = location;
		//}

		bool is_selecting() const {return is_selecting_;}
		sf::Drawable *GetSelectionBoxGraphic() const;
		const sf::Drawable *GetInterfaceGrahic() const;
		const std::list<UnitModel *> GetSelectedUnits() const;

		void set_scene(GameScene *scene) {
			if (game_scene_) delete game_scene_;
			game_scene_ = scene;
		}

		void DeselectDeadUnits();

	private:
    void ConstrainView();
    void MouseScroll();
		const UnitModel *GetUnit(const Vector2f &location) const;
		const std::vector<UnitId> GetUnitIds(const Vector2f &location1,
				const Vector2f &location2) const;
		Vector2i selection_corner1() const {return Util::GetVector2i(selection_corner1_);}
		Vector2i selection_corner2() const {return Util::GetVector2i(selection_corner2_);}
		Vector2f GetGameLocation(const Vector2i &screen_location) const {
			return window.mapPixelToCoords(screen_location, mainView);
		}

		//Vector2i GetGameLocation(const Vector2i &screen_location) const {
		//	return window.mapPixelToCoords(screen_location);
		//}

    sf::RenderWindow& window;
    sf::View mainView, minimapView;
		int mouseHScroll, mouseVScroll, keyboardHScroll, keyboardVScroll;
		void Select(const Vector2i &corner1, const Vector2i &corner2);
		Vector2f selection_corner1_;
		Vector2f selection_corner2_;
		bool is_selecting_;
		std::list<UnitId> selected_unit_ids_;
		CommandTurn command_queue_;
		static const float kScrollSpeed;
		enum CursorAction {kSelect, kAttack};
		CursorAction cursor_action_;
		Vector2f map_size_;
		sf::RectangleShape interface_graphic_;
		GameScene *game_scene_;
		PlayerNumber player_;
};