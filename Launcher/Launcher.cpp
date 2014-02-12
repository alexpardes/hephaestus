#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <Hephaestus/Hephaestus.h>
#include <Hephaestus/GameManager.h>
#include "Launcher.h"
#include "MainMenu.h"

int main() {
  Launcher::Launch();
  return 0;
}

bool Launcher::gameRunning = false;
Hephaestus *Launcher::hephaestus = nullptr;
sf::RenderWindow *Launcher::window = nullptr;

void Launcher::Launch() {
  window = new sf::RenderWindow(sf::VideoMode(1280, 720), "CEGUI Test");
  window->setFramerateLimit(60);
  MainMenu menu;

  while(window->isOpen()) {
    if (!gameRunning) {
      sf::Event event;
      while (window->pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
          window->close();
          break;
        }
        menu.HandleEvent(event);
      }
    }
    if (gameRunning) {
      hephaestus->Update();
    } else {
      menu.Draw();
      window->display();
    }
  }
}

void Launcher::StartSinglePlayerGame(const std::string &map) {
  hephaestus = new Hephaestus(window);
  hephaestus->StartSinglePlayerGame(map);
  gameRunning = true;
}

void Launcher::HostGame(const std::string &map, int port) {
  hephaestus = new Hephaestus(window);
  hephaestus->HostGame(map, port);
  gameRunning = true;
}

void Launcher::JoinGame(const std::string &hostname,
                        const std::string &port) {
  hephaestus = new Hephaestus(window);
  hephaestus->JoinGame(hostname, port);
  gameRunning = true;
}