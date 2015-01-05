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
  resourceManager = new ResourceManager();
  gameManager = new GameManager(kTimestep);
  clock = new sf::Clock();
  gameScene1 = nullptr;
  gameScene2 = nullptr;
  gameInterface = new GameInterface(*window);
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
  gameInterface->SetPlayer(kPlayer1);
  gameManager->SetCommandSource(0, commandBuffer, true);
  gameManager->SetCommandSource(1, CommandSource::Null, true);

  replayWriter.OpenFile(GameFolder() + "replay.rep", map);
  gameManager->AddCommandListener(&replayWriter);
  StartGame();
}

void Hephaestus::PlayReplay(const std::string &replay) {
  std::string map;
  std::getline(std::ifstream(replay), map);
  LoadMap(map);
  gameInterface->SetPlayer(kPlayer1);
  gameManager->SetCommandSource(0, new ReplayReader(replay, 0), false);
  gameManager->SetCommandSource(1, new ReplayReader(replay, 1), false);
  StartGame();
}

void Hephaestus::HostGame(const std::string &map, int port) {
  networkManager->AcceptClient(port,
     [this, map](NetworkConnection *connection) {
       Log::Write("Client connected");
       StartHostedGame(connection, map + ".map");
      });
}

void Hephaestus::StartHostedGame(NetworkConnection *connection,
                                 const std::string &map) {
  assert(connection);
  opponentConnection = connection;
  std::srand((int) std::time(nullptr));
  auto seed = std::rand();
  auto maxLatency = 0.f;
  for (int i = 0; i < 10; ++i) {
    auto latency = connection->Ping();
    maxLatency = std::max(latency, maxLatency);
  }
  // Turn delay is based on one way latency, and we add a margin of 10% to get the 0.55 factor.
  auto turnDelay = (int) (0.55f * maxLatency / kTimestep);
  connection->Send(seed);
  connection->Send(map);
  connection->Send(turnDelay);
  std::srand(seed);
  auto homePlayerSlot = std::rand() % 2;
  auto opponentSlot = 1 - homePlayerSlot;
  LoadMap(map);
  gameManager->SetCommandSource(homePlayerSlot, commandBuffer, true);
  gameManager->SetCommandSource(opponentSlot, opponentConnection);
  commandBuffer->RegisterCommandSink(opponentConnection);
  commandBuffer->CreateTurnDelay(turnDelay);

  gameInterface->SetPlayer(PlayerNumber(homePlayerSlot));
  replayWriter.OpenFile(GameFolder() + "replay.rep", map);
  gameManager->AddCommandListener(&replayWriter);
  StartGame();
}

void Hephaestus::JoinGame(const std::string &hostname,
                          const std::string &port) {
  networkManager->Connect(hostname, port,
      [this](NetworkConnection *connection) {
        Log::Write("Connected to host");
        StartJoinedGame(connection);
      });
}

void Hephaestus::CancelHosting() {
  networkManager->Cancel();
}

void Hephaestus::StartJoinedGame(NetworkConnection *connection) {
  if (connection) {
    opponentConnection = connection;
    for (int i = 0; i < 10; ++i) {
      connection->AwaitPing();
    }
    auto seed = connection->ReadInt();
    auto map = connection->Read();
    auto turnDelay = connection->ReadInt();

    std::srand(seed);
    auto opponentSlot = std::rand() % 2;
    auto homeSlot = 1 - opponentSlot;
    LoadMap(map);
    gameInterface->SetPlayer(PlayerNumber(homeSlot));
    gameManager->SetCommandSource(homeSlot, commandBuffer, true);
    gameManager->SetCommandSource(opponentSlot, opponentConnection);
    commandBuffer->RegisterCommandSink(opponentConnection);
    commandBuffer->CreateTurnDelay(turnDelay);
    replayWriter.OpenFile(GameFolder() + "replay.rep", map);
    gameManager->AddCommandListener(&replayWriter);
    StartGame();
  } else {
    if (observer) observer->OnConnectionFailed();
  }
}

void Hephaestus::LoadMap(const std::string &map) {
  Log::Write("Loading map: " + map);
	gameManager->SetGameState(resourceManager->LoadMap(map));
  Log::Write("Map loaded");
  Vector2i mapSize = resourceManager->MapSize();
  gameInterface->SetMapSize(Util::ToVector2f(mapSize));
  gameInterface->Resize();
}

void Hephaestus::StartGame() {
  gameInterface->Reset();
  graphics = new Graphics(*window, *gameInterface, *resourceManager);
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
    auto command = gameInterface->ProcessEvent(event, *window);
    if (command) {
      Log::Write("Local command processed");
      commandBuffer->AddCommand(command);
    }
  }
}

void Hephaestus::Update() {
  //Log::Write("Update started");
  if (!IsRunning()) {
    if (gameManager->Status() != kFinished)
      gameManager->EndGame();

    Log::Write("Game ended");
    networkManager->Reset();
    if (observer) {
      observer->OnGameEnded(gameManager->Status());
    }
    return;
  }

  float timestep = clock->getElapsedTime().asSeconds();
  clock->restart();
  gameInterface->MoveScreen(timestep);
  GameScene *updated_scene = gameManager->TakeScene();
  if (updated_scene) {
    if (gameScene1) delete gameScene1;
    gameScene1 = gameScene2;
    gameScene2 = updated_scene;
  }
  if (gameScene1 && gameScene2) {
    float weight = gameManager->GetFrameTime();
    GameScene *display_scene = new GameScene(*gameScene1,
      *gameScene2, weight);
    gameInterface->set_scene(display_scene);
    gameInterface->DeselectDeadUnits();

    float framerate = 1 / timestep;
    //Log::Write("Drawing frame");
    graphics->DrawGame(*display_scene, framerate, gameManager->CycleRate());
    //Log::Write("Done drawing frame");
  }
}

std::string Hephaestus::GameFolder() const {
  return std::string(std::getenv("APPDATA")) + "/Firefight/";
}