#pragma once
//#include <thread>
#include <wx/wx.h>
#include <Hephaestus/Hephaestus.h>

namespace sf {
  class RenderWindow;
}

class GameCanvas : public wxWindow /*wxSFMLCanvas*/ {
  public:
    GameCanvas(wxWindow *parent);
    ~GameCanvas();

    void StartGame();
    void Paint();

  private:
    DECLARE_EVENT_TABLE()
    void OnPaint(wxPaintEvent &e);
    void OnIdle(wxIdleEvent &e);

    sf::RenderWindow *window;
    Hephaestus game;
    bool isRunning;
};