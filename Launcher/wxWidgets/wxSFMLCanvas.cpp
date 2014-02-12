#include "stdafx.h"
#include "wxSFMLCanvas.h"

void wxSFMLCanvas::OnIdle(wxIdleEvent&) {
  Refresh();
}

void wxSFMLCanvas::OnPaint(wxPaintEvent&) {
  wxPaintDC dc(this);
  OnUpdate();
  display();
}

wxSFMLCanvas::wxSFMLCanvas(wxWindow* parent) :
    wxWindow(parent, wxID_ANY) {
  sf::RenderWindow::create(GetHandle());
}