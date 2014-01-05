#include "stdafx.h"

#include "Command.h"
#include "GameState.h"
#include "Graphics.h"
#include "Networking.h"
#include "Simulator.h"
#include "GameInterface.h"
#include "GameManager.h"
#include "ResourceManager.h"

const float kTimestep = 100.f;

void SetGameType(GameManager *game_manager, GameType game_type) {

	switch (game_type) {
		case kSinglePlayer:
			game_manager->StartSinglePlayerGame();
			break;
		case kHost:
			game_manager->HostGame(6000);
			break;
		case kJoin: {
			std::string hostname = "127.0.0.1";
			std::ifstream network_settings("network.txt");
			if (network_settings.good()) {
				getline(network_settings, hostname);
			}
			game_manager->JoinGame(hostname, "6000");
			break;
		}
	}
}

void RunSimulation(GameManager *game_manager) {
	game_manager->RunGame();
}

void Quit(sf::RenderWindow &window,
		  GameManager *game_manager,
		  std::thread *thread) {
	if (game_manager) {
		game_manager->EndGame();
		thread->join();
	}
	window.Close();
}

void RunGame(GameType game_type, sf::RenderWindow &window) {
	ResourceManager resource_manager;
	GameManager *game_manager = new GameManager(resource_manager, kTimestep);
	game_manager->InitializeGame();
	SetGameType(game_manager, game_type);
	Vector2i map_size = resource_manager.GetMapSize();
	GameInterface game_interface(Vector2f(1000, 700),
			kTileSize * Util::GetVector2f(map_size), game_manager->self());
	Graphics graphics(window, game_interface, resource_manager);
	GameScene *game_scene1 = NULL;
	GameScene *game_scene2 = NULL;
	std::thread simulation_thread(RunSimulation, game_manager);
	sf::Clock clock;
	float time_step;
	while (window.IsOpened()) {
		time_step = clock.GetElapsedTime();
		clock.Reset();
		sf::Event event;
		while (window.GetEvent(event)) {
			if (event.Type == sf::Event::Closed ||
					(event.Type == sf::Event::KeyPressed &&
					event.Key.Code == sf::Key::Escape)) {
				Quit(window, game_manager, &simulation_thread);
			}
			Command *command = game_interface.ProcessEvent(event);
			if (command) {
				game_manager->BufferCommand(command);
			}
		}
		game_interface.MoveScreen(time_step);
		GameScene *updated_scene = game_manager->TakeScene();
		if (updated_scene) {
			if (game_scene1) delete game_scene1;
			game_scene1 = game_scene2;
			game_scene2 = updated_scene;
		}
		if (game_scene1 && game_scene2) {
			GameScene *display_scene = NULL;
			float weight = game_manager->GetFrameTime();
			display_scene = new GameScene(*game_scene1,
					*game_scene2, weight);
			game_interface.set_scene(display_scene);
			game_interface.DeselectDeadUnits();
			graphics.DrawGame(*display_scene);
		}
	}
}

void Run() {
	sf::RenderWindow window(sf::VideoMode(1000, 700, 32), "Hephaestus"/*,*/
			/*sf::Style::Fullscreen*/);
	window.SetFramerateLimit(60);
	while (window.IsOpened()) {
		sf::Event event;
		while (window.GetEvent(event)) {
			if (event.Type == sf::Event::Closed) window.Close();
			if (event.Type == sf::Event::KeyPressed &&
					event.Key.Code == sf::Key::Escape)
					window.Close();
			if (event.Type == sf::Event::KeyPressed) {
				switch (event.Key.Code) {
					case sf::Key::Num1:
						RunGame(kSinglePlayer, window);
						break;
					case sf::Key::Num2:
						RunGame(kHost, window);
						break;
					case sf::Key::Num3:
						RunGame(kJoin, window);
						break;
				}
			}
		}
	}
}