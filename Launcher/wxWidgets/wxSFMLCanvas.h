#pragma once
#include <wx/wx.h>
#include <SFML/Graphics.hpp>

class wxSFMLCanvas : public wxWindow, public sf::RenderWindow {
  public:
    wxSFMLCanvas(wxWindow* parent = NULL);

  virtual ~wxSFMLCanvas() { }

  protected:
    //DECLARE_EVENT_TABLE()
    virtual void OnUpdate() = 0;
    void OnIdle(wxIdleEvent&);
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&) { }
};