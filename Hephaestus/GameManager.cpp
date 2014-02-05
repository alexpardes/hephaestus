#include "stdafx.h"
#include "GameManager.h"
#include "Commands.h"

GameManager::GameManager(ResourceManager &resource_manager,
						 float timestep) :
		resource_manager_(resource_manager),
		game_state_(NULL),
		last_scene_(NULL),
		buffered_commands_(),
		timestep_(timestep),
		players_(NULL),
		turn_delay_(1) { }

void GameManager::InitializeGame() {
	game_state_ = resource_manager_.LoadMap("default.map");
  players_ = new Players(*game_state_);
	last_scene_ = new GameScene(*game_state_);
}

void GameManager::StartSinglePlayerGame() {
	is_multiplayer_ = false;
	self_ = kPlayer1;
}

void GameManager::HostGame(int port) {
	network_connection_.AcceptClient(port);
	is_multiplayer_ = true;
	self_ = kPlayer1;
	opponent_ = kPlayer2;
}

void GameManager::JoinGame(const std::string &hostname,
						   const std::string &port) {
	network_connection_.Connect(hostname, port);
	is_multiplayer_ = true;
	self_ = kPlayer2;
	opponent_ = kPlayer1;
}

void GameManager::BufferCommand(const Command *command) {
	commands_mutex_.lock();
	buffered_commands_->push_back(command);
	commands_mutex_.unlock();
}

void GameManager::RunGame() {
	Player &home_player = players_->GetPlayer(self_);
	Player &away_player = players_->GetPlayer(opponent_);
  buffered_commands_ = new CommandTurn();

	is_running_ = true;
	while (is_running_) {
		float elapsed_time = clock_.getElapsedTime().asMilliseconds();
		while (elapsed_time < timestep_) {
			boost::this_thread::sleep(boost::posix_time::
					milliseconds(int64_t(timestep_ - elapsed_time)));
			elapsed_time = clock_.getElapsedTime().asMilliseconds();
		};		

		CommandTurn *last_turn = buffered_commands_;

		// I don't think mutex is necessary anymore.
		commands_mutex_.lock();
		buffered_commands_ = new CommandTurn();
		commands_mutex_.unlock();

		if (is_multiplayer_) {
			network_connection_.SendCommands(last_turn);
			if (!turn_delay_) {
				away_player.AddCommandTurn(network_connection_.ReceiveCommands());
			}
		}
		home_player.AddCommandTurn(last_turn);

		if (!turn_delay_) {
			ApplyCommands(kPlayer1);
			ApplyCommands(kPlayer2);
		} else {
			--turn_delay_;
		}

    game_state_->ExecuteTurn();
		scene_mutex_.lock();
		if (last_scene_) delete last_scene_;
		last_scene_ = new GameScene(*game_state_);
		scene_mutex_.unlock();
		clock_.restart();
	}
}

void GameManager::ApplyCommands(PlayerNumber player_number) {
	Player &player = players_->GetPlayer(player_number);
  player.ExecuteTurn();
}