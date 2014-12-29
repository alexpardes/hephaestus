#pragma once
#include <string>
#include <memory>
#include <Hephaestus/HephaestusObserver.h>

class MainMenu;
class Hephaestus;

namespace sf {
  class RenderWindow;
}

class Launcher : public HephaestusObserver {
  public:
    static Launcher *Instance();
    void Launch();
    void StartSinglePlayerGame(const std::string &map);
    void HostGame(const std::string &map, int port);
    void JoinGame(const std::string &hostname, const std::string &port);
    void PlayReplay(const std::string &replay);
    void CancelHosting();

    virtual void OnGameStarted();
    virtual void OnGameEnded(GameStatus status);
    virtual void OnConnectionFailed();

  private:
    Launcher();
    void CreateHephaestus();

    static Launcher *instance;
    std::unique_ptr<Hephaestus> hephaestus;
    sf::RenderWindow *window;
    MainMenu *menu;
    std::string gameFolder;
    bool gameRunning;
    bool isFullscreen;
};