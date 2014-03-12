#include "stdafx.h"
#include "GameManager.h"
#include "Commands.h"
#include "CommandBuffer.h"
#include "NetworkConnection.h"

GameManager::GameManager(float timestep) :
		gameState(NULL),
		lastScene(NULL),
		timestep_(timestep) {
  isRunning = false;
}

void GameManager::SetGameState(GameState *state) {
	gameState = state;
  while (players.size() < 2) {
    players.push_back(new Player(*gameState));
  }

	lastScene = new GameScene(*gameState);
}

void GameManager::SetCommandSource(int playerNumber, CommandSource* source) {
  players[playerNumber]->SetCommandSource(source);
}

void GameManager::StartGame() {
  thread = std::thread(&GameManager::RunGame, this);
}


void GameManager::RunGame() {
	isRunning = true;
	while (isRunning) {

    if (!QueueCommands()) {
      isRunning = false;
      break;
    }

		ApplyCommands();

    gameState->ExecuteTurn();

    float elapsed_time = clock_.getElapsedTime().asMilliseconds();
    while (elapsed_time < timestep_) {
      boost::this_thread::sleep(boost::posix_time::
        milliseconds(int64_t(timestep_ - elapsed_time)));
      elapsed_time = clock_.getElapsedTime().asMilliseconds();
    }

    //clock_.restart();
    currentStepsPerSecond = 1000 / elapsed_time;

		scene_mutex_.lock();
		clock_.restart();
		if (lastScene) delete lastScene;
		lastScene = new GameScene(*gameState);
		scene_mutex_.unlock();
	}

  Reset();
}

void GameManager::Reset() {
  players.clear();
  lastScene = nullptr;
  gameState = nullptr;
}

bool GameManager::QueueCommands() {
  for (Player* player : players) {
    if (!player->QueueCommandTurn()) {
      return false;
    }
  }
  return true;
}

void GameManager::ApplyCommands() {
  for (Player* player : players) {
    player->ExecuteTurn();
  }
}