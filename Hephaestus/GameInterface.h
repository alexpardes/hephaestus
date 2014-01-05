#ifndef GameInterface_
#define GameInterface_

#include "Command.h"
#include "GameState.h"
#include "GameUnit.h"
#include "Networking.h"

class GameInterface {
	public:
		GameInterface(const Vector2f &screen_size,
				const Vector2f &map_size, PlayerNumber player) {
			screen_size_ = screen_size;
			map_size_ = map_size;
			screen_position_ = Vector2f(0.f, 0.f);
			horizontal_scroll_ = 0;
			vertical_scroll_ = 0;
			is_selecting_ = false;
			cursor_action_ = kSelect;
			player_ = player;
			interface_shape_ = sf::Shape::Rectangle(0.f, screen_size.y - 200.f,
					screen_size.x, screen_size.y,
					sf::Color(255, 255, 255, 255));
			game_scene_ = NULL;
		}
		Command *ProcessEvent(const sf::Event &event);
		void MoveScreen(float time_step);

		CommandTurn TakeCommands() {
			CommandTurn old_queue = command_queue_;
			command_queue_ = CommandTurn();
			return old_queue;
		}

		Vector2f screen_position() const {return screen_position_;}
		void set_screen_position(Vector2f location) {
			screen_position_ = location;
		}

		bool is_selecting() const {return is_selecting_;}
		sf::Shape GetSelectionBoxGraphic() const;
		sf::Shape GetInterfaceGrahic() const;
		const std::list<UnitModel *> GetSelectedUnits() const;

		void set_scene(GameScene *scene) {
			if (game_scene_) delete game_scene_;
			game_scene_ = scene;
		}

		void DeselectDeadUnits();

	private:
		const UnitModel *GetUnit(const Vector2i &location) const;
		const std::list<unitId> GetUnitIds(const Vector2f &location1,
				const Vector2f &location2) const;
		Vector2i selection_corner1() const {return Util::GetVector2i(selection_corner1_);}
		Vector2i selection_corner2() const {return Util::GetVector2i(selection_corner2_);}
		Vector2f GetGameLocation(const Vector2f &screen_location) const {
			return screen_location + screen_position_;
		}

		Vector2i GetGameLocation(const Vector2i &screen_location) const {
			return screen_location + Util::GetVector2i(screen_position_);
		}
		//	This is the coordinates of the point of the game world at the top
		//	left corner of the screen.
		Vector2f screen_position_;
		char horizontal_scroll_, vertical_scroll_;
		void Select(const Vector2i &corner1, const Vector2i &corner2);
		Vector2f selection_corner1_;
		Vector2f selection_corner2_;
		bool is_selecting_;
		std::list<unitId> selected_unit_ids_;
		CommandTurn command_queue_;
		static const float kScrollSpeed;
		enum CursorAction {kSelect, kAttack};
		CursorAction cursor_action_;
		Vector2f screen_size_, map_size_;
		sf::Shape interface_shape_;
		GameScene *game_scene_;
		PlayerNumber player_;
};

#endif