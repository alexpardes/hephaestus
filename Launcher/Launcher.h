#pragma once
#include <string>

class Hephaestus;

namespace sf {
  class RenderWindow;
}

class Launcher {
  public:
    static void Launch();
    static void StartSinglePlayerGame(const std::string &map);
    static void HostGame(const std::string &map, int port);
    static void JoinGame(const std::string &hostname, const std::string &port);

  private:
    static Hephaestus *hephaestus;
    static sf::RenderWindow *window;
    static bool gameRunning;
};