#include "stdafx.h"
#include <thread>
#include <fstream>
#include "GameCanvas.h"
#include <Hephaestus/Command.h>
#include <Hephaestus/GameState.h>
#include <Hephaestus/Graphics.h>
#include <Hephaestus/Networking.h>
#include <Hephaestus/Simulator.h>
#include <Hephaestus/GameInterface.h>
#include <Hephaestus/GameManager.h>
#include <Hephaestus/ResourceManager.h>

GameCanvas::GameCanvas(wxWindow *parent) : wxWindow(parent,
                                                    wxID_ANY,
                                                    wxDefaultPosition,
                                                    wxSize(1280, 720)) {
  window = new sf::RenderWindow(GetHandle());
  isRunning = false;
}

GameCanvas::~GameCanvas() {
  if (isRunning) {
    game.Quit();
  }
}

void GameCanvas::OnIdle(wxIdleEvent &e) {
  wxClientDC(this);
  Paint();
  e.RequestMore();
}

void GameCanvas::StartGame() {
  game.RunGame(kSinglePlayer, window);
  isRunning = true;
}

void GameCanvas::OnPaint(wxPaintEvent &e) {
  wxPaintDC(this);
  Paint();
}

void GameCanvas::Paint() {
  if (isRunning) {
    game.Paint();
  }
}

BEGIN_EVENT_TABLE(GameCanvas, wxControl)
  EVT_PAINT(GameCanvas::OnPaint)
  EVT_IDLE(GameCanvas::OnIdle)
END_EVENT_TABLE()