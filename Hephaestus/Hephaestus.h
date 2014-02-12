#pragma once

class GameManager;
class GameScene;
class ResourceManager;
class Graphics;
class GameInterface;
enum GameType;

namespace std {
  class thread;
}

namespace sf {
  class RenderWindow;
  class Clock;
}

class Hephaestus {
  public:
    Hephaestus(sf::RenderWindow *window);
    void StartSinglePlayerGame(const std::string &map);
    void HostGame(const std::string &map, int port);
    void JoinGame(const std::string &hostname, const std::string &port);

    void Update();
    void Quit();

  private:
    void StartGame();
    void LoadMap(const std::string &map);
    static void RunSimulation(GameManager *game_manager);
    sf::RenderWindow *window;
    sf::Clock *clock;
    GameInterface *game_interface;
    GameManager *game_manager;
    Graphics *graphics;
    ResourceManager *resource_manager;
    GameScene *game_scene1, *game_scene2;
    std::thread *simulation_thread;
};