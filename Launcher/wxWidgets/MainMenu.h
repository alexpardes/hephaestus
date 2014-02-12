#pragma once
#include <wx/wx.h>

class LauncherFrame;

class MainMenu : public wxWindow {
  public:
    MainMenu(LauncherFrame *parent);
    ~MainMenu();
    void OnPaint(wxPaintEvent &e);
    void OnSize(wxSizeEvent &e);

  private:
    void OnStartSinglePlayer(wxCommandEvent &e);
    void OnHostGame(wxCommandEvent &e);
    void OnJoinGame(wxCommandEvent &e);

    DECLARE_EVENT_TABLE()
    wxImage backgroundImage;
    wxBitmap backgroundBitmap;
    wxButton *singlePlayerButton, *hostButton, *joinButton;
    LauncherFrame *launcher;
};

wxDEFINE_EVENT(START_GAME, wxCommandEvent);