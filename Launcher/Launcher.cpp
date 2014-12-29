#include "stdafx.h"
#include <boost/filesystem.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Hephaestus/Hephaestus.h>
#include <Hephaestus/GameManager.h>
#include "Launcher.h"
#include "MainMenu.h"

#include <fstream>

int main() {
  try {
    Launcher::Instance()->Launch();
  } catch (std::exception &e) {
    std::ofstream errorLog("ErrorLog.txt");
    errorLog << e.what();
  }
  return 0;
}

Launcher *Launcher::instance = nullptr;

Launcher *Launcher::Instance() {
  if (!instance) {
    instance = new Launcher();
  }
  return instance;
}

sf::RenderWindow *MakeWindow(bool makeFullscreen) {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  if (makeFullscreen) {
    return new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], "Paintball",
      sf::Style::Fullscreen, settings);
  }
  return new sf::RenderWindow(sf::VideoMode(1280, 720), "Paintball", sf::Style::Default, settings);
}

Launcher::Launcher() {
  gameRunning = false;

  gameFolder = std::string(std::getenv("APPDATA")) + "/Firefight/";
  boost::filesystem::create_directory(gameFolder);

#if _DEBUG
  isFullscreen = false;
#else
  isFullscreen = true;
#endif
  window = MakeWindow(isFullscreen);
  window->setFramerateLimit(60);
  menu = new MainMenu(gameFolder);
  hephaestus = nullptr;
}

void Launcher::OnConnectionFailed() {
  menu->OnConnectionFailed();
}

void Launcher::OnGameStarted() {
  gameRunning = true;
}

void Launcher::OnGameEnded(GameStatus status) {
  gameRunning = false;
  menu->OnGameEnded(status);
}

void Launcher::CancelHosting() {
  hephaestus->CancelHosting();
}

void Launcher::Launch() {
  while(window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
        window->close();
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Key::Escape) {
          if (!hephaestus || !hephaestus->IsRunning())
            window->close();
        } else if (event.key.code == sf::Keyboard::Key::F4 &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
          if (hephaestus && hephaestus->IsRunning())
            hephaestus->Stop();

          window->close();
        } else if (event.key.code == sf::Keyboard::Key::Return &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
          delete window;
          isFullscreen = !isFullscreen;
          window = MakeWindow(isFullscreen);
        }
        break;
      }

      if (gameRunning) {
        hephaestus->HandleEvent(event);
      } else {
        menu->HandleEvent(event);
      }
    }
    if (gameRunning) {
      window->pushGLStates();
      hephaestus->Update();
      window->popGLStates();
    } else {
      menu->Draw();
      window->display();
    }
  }
}

void Launcher::CreateHephaestus() {
  hephaestus = std::unique_ptr<Hephaestus>(new Hephaestus(window));
  hephaestus->SetObserver(this);
}

void Launcher::StartSinglePlayerGame(const std::string &map) {
  CreateHephaestus();
  hephaestus->StartSinglePlayerGame(map);
}

void Launcher::HostGame(const std::string &map, int port) {
  CreateHephaestus();
  hephaestus->HostGame(map, port);
}

void Launcher::JoinGame(const std::string &hostname,
                        const std::string &port) {
  auto defaultFilename = gameFolder + "defaults.txt";
  std::ofstream file(defaultFilename);
  file << hostname;

  CreateHephaestus();
  hephaestus->JoinGame(hostname, port);
}

void Launcher::PlayReplay(const std::string &replay) {
  CreateHephaestus();
  hephaestus->PlayReplay(gameFolder + replay);
}