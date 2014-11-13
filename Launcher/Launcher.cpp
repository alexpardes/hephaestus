#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Hephaestus/Hephaestus.h>
#include <Hephaestus/GameManager.h>
#include "Launcher.h"
#include "MainMenu.h"

#include <fstream>

int main() {

  //try {
    Launcher::Instance()->Launch();
  //} catch (std::exception &e) {
  //  std::ofstream errorLog("ErrorLog.txt");
  //  errorLog << e.what();
  //}
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
  sf::Uint32 style = makeFullscreen ? sf::Style::Fullscreen : sf::Style::Default;
  return new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], "Paintball", style, settings);
}

Launcher::Launcher() {
  gameRunning = false;

#if DEBUG
  isFullscreen = false;
#else
  isFullscreen = true;
#endif
  window = MakeWindow(isFullscreen);
  window->setFramerateLimit(60);
  menu = new MainMenu();
  hephaestus = new Hephaestus(window);  
  hephaestus->SetObserver(this);
}

void Launcher::OnConnectionFailed() {
  menu->OnConnectionFailed();
}

void Launcher::OnGameStarted() {
  gameRunning = true;
}

void Launcher::OnGameEnded() {
  gameRunning = false;
  menu->OnGameEnded();
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
        if (event.key.code == sf::Keyboard::Key::F4 &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
          window->close();
        }
        if (event.key.code == sf::Keyboard::Key::Return &&
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

void Launcher::StartSinglePlayerGame(const std::string &map) {
  if (!hephaestus->IsRunning()) {
    hephaestus->StartSinglePlayerGame(map);
  }
}

void Launcher::HostGame(const std::string &map, int port) {
  if (!hephaestus->IsRunning()) {
    hephaestus->HostGame(map, port);
  }
}

void Launcher::JoinGame(const std::string& hostname,
                        const std::string& port) {
  if (!hephaestus->IsRunning()) {
    hephaestus->JoinGame(hostname, port);
  }
}