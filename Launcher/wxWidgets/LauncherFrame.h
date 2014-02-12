#pragma once
#include <wx/wx.h>
#include "MainMenu.h"

class GameCanvas;
class MainMenu;

class LauncherFrame : public wxFrame {
  public:
    LauncherFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    ~LauncherFrame();
    void OnQuit(wxCommandEvent &event);
    void StartGame();

  private:
    wxBoxSizer *sizer;
    GameCanvas *game;
    MainMenu *menu;
    DECLARE_EVENT_TABLE();
};

enum {
  ID_QUIT
};