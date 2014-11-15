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
#include "Replay.h"

const float kTimestep = 50.f;

Hephaestus::Hephaestus(sf::RenderWindow *window) {
  isRunning = false;
  this->window = window;
  resource_manager = new ResourceManager();
  gameManager = new GameManager(kTimestep);
  clock = new sf::Clock();
  game_scene1 = nullptr;
  game_scene2 = nullptr;
  game_interface = new GameInterface(*window);
  commandBuffer = new CommandBuffer();
  networkManager = new NetworkManager();
  observer = nullptr;
}

bool Hephaestus::IsRunning() const {
  return gameManager->Status() == kRunning;
}

void Hephaestus::Stop() {
  gameManager->EndGame();
}

void Hephaestus::SetObserver(HephaestusObserver* observer) {
  this->observer = observer;
}

void Hephaestus::StartSinglePlayerGame(const std::string &map) {
  LoadMap(map);
  game_interface->SetPlayer(kPlayer1);
  gameManager->SetCommandSource(0, commandBuffer, true);
  gameManager->SetCommandSource(1, CommandSource::Null, true);
  gameManager->SetSaveReplay(true);
  StartGame();
}

void Hephaestus::PlayReplay(const std::string &replay) {
  // TODO: Get map from replay file.
  LoadMap("default.map");
  game_interface->SetPlayer(kPlayer1);
  gameManager->SetCommandSource(0, new ReplayReader(replay, 0), false);
  gameManager->SetCommandSource(1, new ReplayReader(replay, 1), false);
  gameManager->SetSaveReplay(false);
  StartGame();
}

void Hephaestus::HostGame(const std::string &map, int port) {
  networkManager->AcceptClient(port,
     [this, map](NetworkConnection* connection) {
       Log::Write("Client connected");
       StartHostedGame(connection, map);
      });
}

void Hephaestus::StartHostedGame(NetworkConnection* connection,
                                 const std::string& map) {
  assert(connection);
  LoadMap(map);
  opponentConnection = connection;
  gameManager->SetCommandSource(0, commandBuffer, true);
  gameManager->SetCommandSource(1, opponentConnection);
  commandBuffer->RegisterCommandSink(opponentConnection);
  commandBuffer->CreateTurnDelay(1);

  game_interface->SetPlayer(kPlayer1);
  gameManager->SetSaveReplay(true);
  StartGame();
}

void Hephaestus::JoinGame(const std::string& hostname,
                          const std::string& port) {
  LoadMap("default.map"); // TODO: get map choice from host.
  networkManager->Connect(hostname, port,
      [this](NetworkConnection* connection) {
        Log::Write("Connected to host");
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
    gameManager->SetCommandSource(0, opponentConnection);
    gameManager->SetCommandSource(1, commandBuffer, true);
    commandBuffer->RegisterCommandSink(opponentConnection);
    commandBuffer->CreateTurnDelay(1);
    gameManager->SetSaveReplay(true);
    StartGame();
  } else {
    if (observer) observer->OnConnectionFailed();
  }
}

void Hephaestus::LoadMap(const std::string &map) {
  Log::Write("Loading map");
	gameManager->SetGameState(resource_manager->LoadMap("default.map"));
  Log::Write("Map loaded");
  Vector2i map_size = resource_manager->MapSize(); 
  game_interface->SetMapSize(kTileSize * Util::ToVector2f(map_size));
  game_interface->Resize();
}

void Hephaestus::StartGame() {
  graphics = new Graphics(*window, *game_interface, *resource_manager);
  gameManager->StartGame();
  isRunning = true;
  if (observer) {
    observer->OnGameStarted();
  }
}

void Hephaestus::HandleEvent(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed
      && event.key.code == sf::Keyboard::Escape) {
    gameManager->EndGame();
  } else {
    auto command = game_interface->ProcessEvent(event, *window);
    if (command) {
      Log::Write("Local command processed");
      commandBuffer->AddCommand(command);
    }
  }
}

void Hephaestus::Update() {
  //Log::Write("Update started");
  if (!IsRunning()) {
    Log::Write("Game ended");
    networkManager->Reset();
    if (observer) {
      observer->OnGameEnded(gameManager->Status());
    }
    return;
  }

  float timestep = clock->getElapsedTime().asSeconds();
  clock->restart();
  game_interface->MoveScreen(timestep);
  GameScene *updated_scene = gameManager->TakeScene();
  if (updated_scene) {
    if (game_scene1) delete game_scene1;
    game_scene1 = game_scene2;
    game_scene2 = updated_scene;
  }
  if (game_scene1 && game_scene2) {
    float weight = gameManager->GetFrameTime();
    GameScene *display_scene = new GameScene(*game_scene1,
      *game_scene2, weight);
    game_interface->set_scene(display_scene);
    game_interface->DeselectDeadUnits();

    float framerate = 1 / timestep;
    //Log::Write("Drawing frame");
    graphics->DrawGame(*display_scene, framerate, gameManager->CycleRate());
    //Log::Write("Done drawing frame");
  }
}