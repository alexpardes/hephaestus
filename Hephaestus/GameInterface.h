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
		GameInterface(sf::RenderWindow &window);

    void Resize();
    void Reset();

    const sf::View &MainView() const { return mainView; }
    const sf::View &MinimapView() const { return minimapView; }

		std::shared_ptr<Command> ProcessEvent(const sf::Event &event, const sf::RenderWindow &window);
		void MoveScreen(float time_step);

    void SetMapSize(const Vector2f &mapSize) {
      
      int screenWidth = window.getSize().x;
      int screenHeight = window.getSize().y;

      this->mapSize = mapSize;
      minimapView.setCenter(mapSize / 2.f);
      minimapView.setSize(mapSize);
      float mapAspectRatio = mapSize.x / mapSize.y;
      float screenAspectRatio = float(screenWidth) / screenHeight;
      minimapView.setViewport(sf::FloatRect(0.01f / screenAspectRatio, 0.69f,
          0.3f * mapAspectRatio / screenAspectRatio, 0.3f));
    }

    void SetPlayer(PlayerNumber player) {
      this->player = player;
    }

    PlayerNumber Player() const { return player; }

		//Vector2f screen_position() const {return screen_position_;}
		//void set_screen_position(Vector2f location) {
		//	screen_position_ = location;
		//}

		bool is_selecting() const {return isSelecting;}
		sf::Drawable *GetSelectionBoxGraphic() const;
		const sf::Drawable *GetInterfaceGrahic() const;
		const std::list<UnitModel *> GetSelectedUnits() const;

		void set_scene(GameScene *scene) {
			if (gameScene) delete gameScene;
			gameScene = scene;
		}

		void DeselectDeadUnits();

	private:
    static bool hasRegisteredCommands;
    static void RegisterCommands();
    void ConstrainView();
    void MouseScroll();
		const UnitModel *GetUnit(const Vector2f &location) const;
		const std::vector<UnitId> GetUnitIds(const Vector2f &location1,
				const Vector2f &location2) const;
		Vector2i selection_corner1() const {return Util::GetVector2i(selectionCorner1);}
		Vector2i selection_corner2() const {return Util::GetVector2i(selectionCorner2);}
		Vector2f GetGameLocation(const Vector2i &screen_location) const {
			return window.mapPixelToCoords(screen_location, mainView);
		}

		//Vector2i GetGameLocation(const Vector2i &screen_location) const {
		//	return window.mapPixelToCoords(screen_location);
		//}

    sf::RenderWindow &window;
    sf::View mainView, minimapView;
		int mouseHScroll, mouseVScroll, keyboardHScroll, keyboardVScroll;
		void Select(const Vector2i &corner1, const Vector2i &corner2);
		Vector2f selectionCorner1;
		Vector2f selectionCorner2;
		bool isSelecting;
		std::list<UnitId> selectedUnitIds;
		static const float kScrollSpeed;
		enum CursorAction {kSelect, kAttack, kTarget};
		CursorAction cursorAction;
		Vector2f mapSize;
		sf::RectangleShape interfaceGraphic;
		GameScene *gameScene;
		PlayerNumber player;
};