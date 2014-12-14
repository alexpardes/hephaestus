#pragma once
#include <mutex>
#include <thread>
#include "GameStatus.h"
#include "Replay.h"

class CommandSource;
class GameScene;
class GameState;
class Player;

class GameManager {
	public:
		GameManager(float timestep);
    void SetCommandSource(int playerNumber, CommandSource* source, bool isLocal = false);
		void SetGameState(GameState *state);
    void StartGame();
		void EndGame();
    void SetSaveReplay(bool saveReplay);
		GameScene *TakeScene();
    GameStatus Status() const { return status; }
		float GetFrameTime();
    float CycleRate() const { return currentStepsPerSecond; }

	private:
    enum Result {
      kSuccess,
      kDesync,
      kConnectionFailure
    };

    std::string GameFolder() const;
		Result ApplyCommands();
    Result QueueCommands();
    void RunGame();
    void Reset();
    void SetHash();

		GameState *gameState;
		GameScene *lastScene;
		const float timestep;
		sf::Clock clock;
		GameStatus status;
		std::mutex sceneMutex;
    std::vector<Player*> players;
    std::thread thread;
    std::vector<CommandSource*> localCommandSources;
    float currentStepsPerSecond;
    ReplayWriter replayWriter;
    bool saveReplay;

};