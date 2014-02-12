#include "stdafx.h"
#include "LauncherFrame.h"
#include "GameCanvas.h"
#include "MainMenu.h"
#include "resource.h"

LauncherFrame::LauncherFrame(const wxString &title,
                             const wxPoint &pos,
                             const wxSize &size) :
    wxFrame(NULL, wxID_ANY, title, pos, size) {

  SetIcon(wxIcon(wxT("GameIconAlpha.ico")));


  menu = new MainMenu(this);
  game = new GameCanvas(this);

  sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(menu);
  sizer->Add(game);

  game->Hide();
  SetSizer(sizer);
}

LauncherFrame::~LauncherFrame() {
  delete menu;
  delete game;
  delete sizer;
}

void LauncherFrame::OnQuit(wxCommandEvent &event) {
  Close(true);
}

void LauncherFrame::StartGame() {
  game->StartGame();
  game->Paint();
  menu->Hide();
  game->Show();
}

BEGIN_EVENT_TABLE(LauncherFrame, wxFrame)
  EVT_MENU(ID_QUIT, LauncherFrame::OnQuit)
END_EVENT_TABLE()