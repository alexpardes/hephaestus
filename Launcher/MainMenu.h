#pragma once
#include <unordered_map>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cegui/CEGUI.h>
#include <cegui/RendererModules/OpenGL/GLRenderer.h>

namespace sf {
  class Event;
}

class MainMenu {
  public:
    MainMenu();
    void Draw();
    void HandleEvent(sf::Event &event);

  private:
    CEGUI::OpenGLRenderer *renderer;

    static CEGUI::GUIContext &defaultContext() {
      return CEGUI::System::getSingleton().getDefaultGUIContext();
    }
    std::unordered_map<sf::Keyboard::Key, CEGUI::Key::Scan> mKeyMap;
    std::unordered_map<sf::Mouse::Button, CEGUI::MouseButton> mMouseButtonMap;

    void initializeMaps();
    CEGUI::Key::Scan toCEGUIKey(sf::Keyboard::Key Code);
    CEGUI::MouseButton toCEGUIMouseButton(sf::Mouse::Button Button);
};