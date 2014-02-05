#ifndef GAMEMANAGER_
#define GAMEMANAGER_

#include "Networking.h"
#include "Players.h"
#include "ResourceManager.h"

enum GameType {kSinglePlayer, kHost, kJoin};

class GameManager {
	public:
		GameManager(ResourceManager &resource_manager, float timestep);
		void StartSinglePlayerGame();
		void HostGame(int port);
		void JoinGame(const std::string &hostname, const std::string &port);
		void BufferCommand(const Command *command);
		void InitializeGame();
		void EndGame() {is_running_ = false;}

		float GetFrameTime() {
			float frame_time;
			if (last_scene_) {
				frame_time = 1.f;
			} else {
				frame_time = clock_.getElapsedTime().asMilliseconds() / timestep_;
			}
			return frame_time;
		}

		GameScene *TakeScene() {
			GameScene *scene = NULL;
			if (last_scene_) {
				scene_mutex_.lock();
				scene = last_scene_;
				last_scene_ = NULL;
				scene_mutex_.unlock();
			}
			return scene;
		}

		//std::list<std::shared_ptr<GameUnit> > GetSelectedUnits() {
		//	return players_.GetPlayer(self_).selected_units();
		//}

		PlayerNumber self() const {return self_;}
		void RunGame();

	private:
		void ApplyCommands(PlayerNumber player_number);
		ResourceManager &resource_manager_;
		GameState *game_state_;
		GameScene *last_scene_;
		NetworkConnection network_connection_;
		CommandTurn *buffered_commands_;
		const float timestep_;
		bool is_multiplayer_;
		sf::Clock clock_;
		bool is_running_;
		std::mutex commands_mutex_;
		std::mutex scene_mutex_;
		Players *players_;
		PlayerNumber self_, opponent_;
		int turn_delay_;
};

#endif