#pragma once
#include "GameStatus.h"

class HephaestusObserver {
  public:
    virtual void OnGameStarted() = 0;
    virtual void OnGameEnded(GameStatus result) = 0;
    virtual void OnConnectionFailed() = 0;
    virtual ~HephaestusObserver() { }
};