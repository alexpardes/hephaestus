#include "stdafx.h"
#include "Hephaestus.h"
#include "Command.h"
#include "GameState.h"
#include "Graphics.h"
#include "NetworkManager.h"
#include "GameInterface.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "CommandBuffer.h"
#include "NetworkConnection.h"

const float kTimestep = 100.f;

Hephaestus::Hephaestus(sf::RenderWindow *window) {
  isRunning = false;
  this->window = window;
  resource_manager = new ResourceManager();
  game_manager = new GameManager(kTimestep);
  clock = new sf::Clock();
  game_scene1 = nullptr;
  game_scene2 = nullptr;
  game_interface = new GameInterface(*window);
  commandBuffer = new CommandBuffer();
  networkManager = new NetworkManager();
  observer = nullptr;
}

bool Hephaestus::IsRunning() const {
  return game_manager->IsRunning();
}

void Hephaestus::SetObserver(HephaestusObserver* observer) {
  this->observer = observer;
}

void Hephaestus::RunSimulation(GameManager* game_manager) {
	//game_manager->RunGame();
}

void Hephaestus::StartSinglePlayerGame(const std::string &map) {
  LoadMap(map);
  game_interface->SetPlayer(kPlayer1);
  game_manager->SetCommandSource(0, commandBuffer);
  StartGame();
}

void Hephaestus::HostGame(const std::string &map, int port) {
  networkManager->AcceptClient(port,
     [this, map](NetworkConnection* connection) {
       StartHostedGame(connection, map);
      });
}

void Hephaestus::StartHostedGame(NetworkConnection* connection,
                                 const std::string& map) {
  assert(connection);
  LoadMap(map);
  opponentConnection = connection;
  game_manager->SetCommandSource(0, commandBuffer);
  game_manager->SetCommandSource(1, opponentConnection);
  commandBuffer->RegisterCommandSink(opponentConnection);
  commandBuffer->CreateTurnDelay(1);

  game_interface->SetPlayer(kPlayer1);
  StartGame();
}

void Hephaestus::JoinGame(const std::string& hostname,
                          const std::string& port) {
  LoadMap("default.map"); // TODO: get map choice from host.
  networkManager->Connect(hostname, port,
      [this](NetworkConnection* connection) {
        StartJoinedGame(connection);
      });
}

void Hephaestus::CancelHosting() {
  networkManager->Cancel();
}

void Hephaestus::StartJoinedGame(NetworkConnection* connection) {
  if (connection) {
    opponentConnection = connection;
    game_interface->SetPlayer(kPlayer2);
    game_manager->SetCommandSource(0, opponentConnection);
    game_manager->SetCommandSource(1, commandBuffer);
    commandBuffer->RegisterCommandSink(opponentConnection);
    commandBuffer->CreateTurnDelay(1);
    StartGame();
  } else {
    if (observer) observer->OnConnectionFailed();
  }
}

void Hephaestus::LoadMap(const std::string &map) {
	game_manager->SetGameState(resource_manager->LoadMap("default.map"));
  Vector2i map_size = resource_manager->MapSize(); 
  game_interface->SetMapSize(kTileSize * Util::ToVector2f(map_size));
  game_interface->Resize();
}

void Hephaestus::StartGame() {
  graphics = new Graphics(*window, *game_interface, *resource_manager);
  game_manager->StartGame();
  isRunning = true;
  if (observer) {
    observer->OnGameStarted();
  }
}

void Hephaestus::Update() {
  if (!IsRunning()) {
    networkManager->Reset();
    game_manager->EndGame();
    if (observer) {
      observer->OnGameEnded();
    }
    return;
  }

  float timestep = clock->getElapsedTime().asSeconds();
  clock->restart();
  sf::Event event;
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
        game_manager->EndGame();
        window->close();
    }
    Command *command = game_interface->ProcessEvent(event, *window);
    if (command) {
      commandBuffer->AddCommand(command);
    }
  }
  game_interface->MoveScreen(timestep);
  GameScene *updated_scene = game_manager->TakeScene();
  if (updated_scene) {
    if (game_scene1) delete game_scene1;
    game_scene1 = game_scene2;
    game_scene2 = updated_scene;
    updated_scene->ComputeVisibility(game_interface->Player());
  }
  if (game_scene1 && game_scene2) {
    GameScene *display_scene = NULL;
    float weight = game_manager->GetFrameTime();
    display_scene = new GameScene(*game_scene1,
      *game_scene2, weight);
    game_interface->set_scene(display_scene);
    game_interface->DeselectDeadUnits();
    graphics->DrawGame(*display_scene, timestep);
  }
}