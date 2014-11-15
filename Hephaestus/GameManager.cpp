#include "stdafx.h"
#include "GameManager.h"
#include "Commands.h"
#include "CommandBuffer.h"
#include "CommandTurn.h"
#include "NetworkConnection.h"
#include "Player.h"
#include "Timer.h"

GameManager::GameManager(float timestep) :
		gameState(nullptr),
		lastScene(nullptr),
		timestep(timestep) {

  saveReplay = true;
  status = kFinished;
}

void GameManager::SetGameState(GameState *state) {
	gameState = state;
  while (players.size() < 2) {
    players.push_back(new Player(*gameState));
  }

	lastScene = new GameScene(*gameState);
}

void GameManager::SetCommandSource(int playerNumber, CommandSource* source, bool isLocal) {
  players[playerNumber]->SetCommandSource(source);
  if (isLocal)
    localCommandSources.push_back(source);
}

void GameManager::StartGame() {
  status = kRunning;
  thread = std::thread(&GameManager::RunGame, this);
}


void GameManager::RunGame() {
  Log::Write("Game started");
  if (saveReplay)
    replayWriter.OpenFile("replay.rep");

	while (status == kRunning) {
    SetHash();

    if (QueueCommands() != kSuccess) {
      Log::Write("Connection lost");
      status = kDisconnected;
      break;
    }

		if (ApplyCommands() != kSuccess) {
      Log::Write("Desync");
      status = kDesynced;
      break;
    }
    
    //Timer::Start();
    gameState->ExecuteTurn();
    //Timer::Stop();

    float elapsed_time = clock.getElapsedTime().asSeconds() * 1000;
    while (elapsed_time < timestep) {
      boost::this_thread::sleep(boost::posix_time::
          milliseconds(int64_t(timestep - elapsed_time)));
      elapsed_time = clock.getElapsedTime().asSeconds() * 1000;
    }

    currentStepsPerSecond = 1000 / elapsed_time;

		sceneMutex.lock();
		clock.restart();
		if (lastScene) delete lastScene;
		lastScene = new GameScene(*gameState);
		sceneMutex.unlock();
	}

  Reset();
}

void GameManager::Reset() {
  players.clear();
  lastScene = nullptr;
  gameState = nullptr;
}

auto GameManager::QueueCommands() -> Result {
  for (Player* player : players) {
    if (!player->QueueCommandTurn()) {
      return kConnectionFailure;
    }
  }
  return kSuccess;
}

auto GameManager::ApplyCommands() -> Result {
  auto gameHash = players[0]->NextTurn().GameHash();
  for (Player* player : players) {
    const CommandTurn &turn = player->NextTurn();
    if (saveReplay)
      replayWriter.WriteTurn(turn);

    if (turn.GameHash() != gameHash)
      return kDesync;

    player->ExecuteTurn();
  }
  return kSuccess;
}

void GameManager::EndGame() {
  if (status == kRunning)
    status = kFinished;

  if (saveReplay) {
    Log::Write("Creating human readable replay");
    replayWriter.CloseFile();
    ReplayReader("replay.rep").WriteHumanReadable("replay.hrr");
  }
  thread.join();
}

float GameManager::GetFrameTime() {
  float frameTime;
  sceneMutex.lock();
  if (lastScene) {
    frameTime = 1.f;
  } else {
    frameTime = std::min(clock.getElapsedTime().asMilliseconds() / timestep, 1.f);
  }
  sceneMutex.unlock();
  return frameTime;
}

GameScene *GameManager::TakeScene() {
  GameScene *scene = nullptr;
  if (lastScene) {
    sceneMutex.lock();
    scene = lastScene;
    lastScene = nullptr;
    sceneMutex.unlock();
  }
  return scene;
}

void GameManager::SetHash() {
  size_t gameHash = gameState->HashCode();
  for (auto commandSource : localCommandSources) {
    commandSource->SetGameHash(gameHash);
  }
}

void GameManager::SetSaveReplay(bool saveReplay) {
  this->saveReplay = saveReplay;
}