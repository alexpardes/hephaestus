#pragma once
#include <string>
#include <Hephaestus/HephaestusObserver.h>

class MainMenu;
class Hephaestus;

namespace sf {
  class RenderWindow;
}

class Launcher : public HephaestusObserver {
  public:
    static Launcher* Instance();
    void Launch();
    void StartSinglePlayerGame(const std::string& map);
    void HostGame(const std::string& map, int port);
    void JoinGame(const std::string& hostname, const std::string& port);
    void CancelHosting();

    void OnConnectionFailed();
    void OnGameStarted();
    void OnGameEnded();

  private:
    Launcher();
    static Launcher* instance;
    Hephaestus* hephaestus;
    sf::RenderWindow *window;
    MainMenu* menu;
    bool gameRunning;
};