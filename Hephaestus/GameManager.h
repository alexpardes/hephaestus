#pragma once

#include "Player.h"
#include <mutex>
#include <thread>

class CommandSource;

class GameManager {
	public:
		GameManager(float timestep);
    void SetCommandSource(int playerNumber, CommandSource* source);
		void SetGameState(GameState *state);
		void EndGame() {
      isRunning = false;
      thread.join();
    }

		float GetFrameTime() {
			float frame_time;
			if (lastScene) {
				frame_time = 1.f;
			} else {
				frame_time = clock_.getElapsedTime().asMilliseconds() / timestep_;
			}
			return frame_time;
		}

		GameScene *TakeScene() {
			GameScene *scene = NULL;
			if (lastScene) {
				scene_mutex_.lock();
				scene = lastScene;
				lastScene = NULL;
				scene_mutex_.unlock();
			}
			return scene;
		}

    void StartGame();
    bool IsRunning() const { return isRunning; }

	private:
		void ApplyCommands();
    bool QueueCommands();
    void RunGame();
    void Reset();
		GameState *gameState;
		GameScene *lastScene;
		const float timestep_;
		sf::Clock clock_;
		bool isRunning;
		std::mutex scene_mutex_;
    std::vector<Player*> players;
    std::thread thread;
};