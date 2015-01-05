#pragma once
#include "HephaestusObserver.h"
#include "Replay.h"

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
    void PlayReplay(const std::string &replay);
    void HostGame(const std::string &map, int port);
    void JoinGame(const std::string &hostname, const std::string &port);
    void CancelHosting();
    void SetObserver(HephaestusObserver* observer);
    void HandleEvent(const sf::Event &event);
    void Update();
    void Stop();
    bool IsRunning() const;

  private:
    void StartHostedGame(NetworkConnection *connection, const std::string &map);
    void StartJoinedGame(NetworkConnection *connection);
    void StartGame();
    void LoadMap(const std::string &map);
    std::string GameFolder() const;
    sf::RenderWindow *window;
    sf::Clock *clock;
    GameInterface *gameInterface;
    GameManager *gameManager;
    Graphics *graphics;
    ResourceManager *resourceManager;
    GameScene *gameScene1, *gameScene2;
    CommandBuffer *commandBuffer;
    NetworkManager *networkManager;
    NetworkConnection *opponentConnection;
    HephaestusObserver *observer;
    ReplayWriter replayWriter;
    bool isRunning;
};