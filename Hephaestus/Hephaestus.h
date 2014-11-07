#pragma once
#include "HephaestusObserver.h"

class GameManager;
class GameScene;
class ResourceManager;
class Graphics;
class GameInterface;
class CommandBuffer;
class NetworkManager;
class NetworkConnection;

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
    void JoinGame(const std::string &hostname, const std::string& port);
    void CancelHosting();
    void SetObserver(HephaestusObserver* observer);

    void Update();
    bool IsRunning() const;

  private:
    void StartHostedGame(NetworkConnection* connection, const std::string& map);
    void StartJoinedGame(NetworkConnection* connection);
    void StartGame();
    void LoadMap(const std::string &map);
    sf::RenderWindow *window;
    sf::Clock *clock;
    GameInterface *game_interface;
    GameManager *game_manager;
    Graphics *graphics;
    ResourceManager *resource_manager;
    GameScene *game_scene1, *game_scene2;
    CommandBuffer *commandBuffer;
    NetworkManager *networkManager;
    NetworkConnection *opponentConnection;
    HephaestusObserver *observer;
    bool isRunning;
};