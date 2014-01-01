#include "stdafx.h"
#include "GameManager.h"

GameManager::GameManager(ResourceManager &resource_manager,
						 float timestep) :
		resource_manager_(resource_manager),
		game_state_(NULL),
		last_scene_(NULL),
		buffered_commands_(),
		timestep_(timestep),
		players_(),
		turn_delay_(1) { }

void GameManager::InitializeGame() {
	game_state_ = resource_manager_.LoadMap("default.map");
	last_scene_ = new GameScene(*game_state_);
	simulator_ = new Simulator(*game_state_, timestep_);
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
	Player &home_player = players_.GetPlayer(self_);
	Player &away_player = players_.GetPlayer(opponent_);

	is_running_ = true;
	while (is_running_) {
		float elapsed_time = clock_.GetElapsedTime() * 1000.f;
		while (elapsed_time < timestep_) {
			boost::this_thread::sleep(boost::posix_time::
					milliseconds(int64_t(timestep_ - elapsed_time)));
			elapsed_time = clock_.GetElapsedTime() * 1000.f;
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

		simulator_->StepSimulation();
		scene_mutex_.lock();
		if (last_scene_) delete last_scene_;
		last_scene_ = new GameScene(*game_state_);
		scene_mutex_.unlock();
		clock_.Reset();
	}
}

void GameManager::IssueMoveCommand(const std::list<GameUnit *> &units,
								   const Vector2i &location) {
	for (std::list<GameUnit *>::const_iterator p_unit =
			units.begin(); p_unit != units.end();
			++p_unit) {
		(*p_unit)->set_destination(Util::GetVector2f(location));
		(*p_unit)->set_target(NULL);
		(*p_unit)->ClearPath();
	}
}

void GameManager::IssueAttackCommand(const std::list<GameUnit *> &units,
									 unitId target_id) {
	GameUnit *target = game_state_->GetUnit(target_id);
	for (std::list<GameUnit *>::const_iterator p_unit =
			units.begin(); p_unit != units.end();
			++p_unit) {
		(*p_unit)->set_target(target);
		(*p_unit)->set_destination((*p_unit)->position());
		(*p_unit)->ClearPath();
	}
}

void GameManager::ApplyCommands(PlayerNumber player_number) {
	Player &player = players_.GetPlayer(player_number);
	player.DeselectDeadUnits();
	const CommandTurn *commands = player.PopCommandTurn();
	if (commands) {
		for (int i = 0; i < commands->size(); ++i) {
			const Command *command = (*commands)[i];
			switch (command->type()) {
				case Command::kMove: {
					const Vector2i &location =
							static_cast<const MoveCommand *>(command)->location();
					IssueMoveCommand(player.selected_units(), location);
					break;
				}
				case Command::kAttack: {
					unitId target_id =
							static_cast<const AttackCommand *>(command)->target();
					IssueAttackCommand(player.selected_units(), target_id);
					break;
				}
				case Command::kSelect: {
					std::list<GameUnit *> selected_units;
					std::list<unitId> unit_ids =
							static_cast<const SelectCommand *>(command)->targets();
					std::list<GameUnit *> units;
					for (std::list<unitId>::iterator id = unit_ids.begin();
							id != unit_ids.end(); ++id) {
						selected_units.push_back(game_state_->GetUnit(*id));
					}
					player.set_selected_units(selected_units);
					break;
				}
			}
			delete command;
		}
		delete commands;
	}
}