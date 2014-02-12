#include "stdafx.h"
#include "MainMenu.h"
#include "LauncherFrame.h"
#include "HostGameForm.h"

enum {
  SINGLE_PLAYER,
  HOST,
  JOIN
};

MainMenu::MainMenu(LauncherFrame *parent) : wxWindow(parent,
                                                     wxID_ANY,
                                                     wxDefaultPosition,
                                                     wxSize(1920, 1080)) {
  launcher = parent;
  backgroundImage = wxImage("background.jpg");
  backgroundBitmap = wxBitmap(backgroundImage);

  wxFlexGridSizer *sizer = new wxFlexGridSizer(1, 10, 10);
  sizer->AddGrowableCol(0);

  wxFont buttonFont(wxFont(wxFontInfo(18).FaceName("Verdana").Bold()));
  singlePlayerButton = new wxButton(this, SINGLE_PLAYER, _T("Single Player"));
  singlePlayerButton->SetFont(buttonFont);
  singlePlayerButton->SetInitialSize();
  sizer->Add(singlePlayerButton, 1, wxEXPAND);

  hostButton = new wxButton(this, HOST, _T("Host Game"));
  hostButton->SetFont(buttonFont);
  hostButton->SetInitialSize();
  sizer->Add(hostButton, 1, wxEXPAND);
  sizer->Add(new HostGameForm(this), 1, wxEXPAND);

  joinButton = new wxButton(this, JOIN, _T("Join Game"));
  joinButton->SetFont(buttonFont);
  joinButton->SetBestFittingSize();
  sizer->Add(joinButton, 1, wxEXPAND);

  SetSizer(sizer);
  Layout();
}

MainMenu::~MainMenu() {
  //delete singlePlayerButton;
}


void MainMenu::OnPaint(wxPaintEvent &e) {
  wxPaintDC dc(this);
  if (dc.GetSize() != backgroundBitmap.GetSize() ) {
    backgroundBitmap = wxBitmap(backgroundImage.Scale(dc.GetSize().GetX(),
        dc.GetSize().GetY()));
  }
  dc.DrawBitmap(backgroundBitmap, 0, 0);
}

void MainMenu::OnSize(wxSizeEvent &e) {
  Refresh();
}

void MainMenu::OnStartSinglePlayer(wxCommandEvent &e) {
  launcher->StartGame();
}

void MainMenu::OnHostGame(wxCommandEvent &e) {
  launcher->StartGame();
}

void MainMenu::OnJoinGame(wxCommandEvent &e) {
  launcher->StartGame();
}

BEGIN_EVENT_TABLE(MainMenu, wxWindow)
  EVT_PAINT(MainMenu::OnPaint)
  EVT_SIZE(MainMenu::OnSize)
  EVT_BUTTON(SINGLE_PLAYER, MainMenu::OnStartSinglePlayer)
  EVT_BUTTON(HOST, MainMenu::OnHostGame)
  EVT_BUTTON(JOIN, MainMenu::OnJoinGame)
END_EVENT_TABLE()
