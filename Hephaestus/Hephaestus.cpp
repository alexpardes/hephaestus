#include "stdafx.h"
#include "Hephaestus.h"
#include "Command.h"
#include "GameState.h"
#include "Graphics.h"
#include "Networking.h"
#include "Simulator.h"
#include "GameInterface.h"
#include "GameManager.h"
#include "ResourceManager.h"

const float kTimestep = 100.f;

Hephaestus::Hephaestus(sf::RenderWindow *window) {
  this->window = window;
  resource_manager = new ResourceManager();
  game_manager = new GameManager(*resource_manager, kTimestep);
  clock = new sf::Clock();
  game_scene1 = NULL;
  game_scene2 = NULL;
}

void Hephaestus::RunSimulation(GameManager *game_manager) {
	game_manager->RunGame();
}

void Hephaestus::Quit() {
	if (game_manager) {
		game_manager->EndGame();
		simulation_thread->join();
	}
	window->close();
}

void Hephaestus::StartSinglePlayerGame(const std::string &map) {
  LoadMap(map);
  game_manager->StartSinglePlayerGame();
  StartGame();
}

void Hephaestus::HostGame(const std::string &map, int port) {
  LoadMap(map);
  game_manager->HostGame(port);
  StartGame();
}

void Hephaestus::JoinGame(const std::string &hostname,
                          const std::string &port) {
  LoadMap("default.map"); // TODO: get map choice from host.
  game_manager->JoinGame(hostname, port);
  StartGame();
}

void Hephaestus::LoadMap(const std::string &map) {
	game_manager->SetGameState(resource_manager->LoadMap("default.map"));
}

void Hephaestus::StartGame() {
  Vector2i map_size = resource_manager->GetMapSize(); 
  game_interface = new GameInterface(Vector2f(1280, 720),
    kTileSize * Util::GetVector2f(map_size), game_manager->self());
  graphics = new Graphics(*window, *game_interface, *resource_manager);

  simulation_thread = new std::thread(RunSimulation, game_manager);
}

void Hephaestus::Update() {
  float time_step = clock->getElapsedTime().asSeconds();
  clock->restart();
  sf::Event event;
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
        Quit();
    }
    Command *command = game_interface->ProcessEvent(event, *window);
    if (command) {
      game_manager->BufferCommand(command);
    }
  }
  game_interface->MoveScreen(time_step);
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
    game_interface->set_scene(display_scene);
    game_interface->DeselectDeadUnits();
    graphics->DrawGame(*display_scene);
  }
}