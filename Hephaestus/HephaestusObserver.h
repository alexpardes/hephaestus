#pragma once

class HephaestusObserver {
  public:
    virtual void OnConnectionFailed() = 0;
    virtual void OnGameStarted() = 0;
    virtual void OnGameEnded() = 0;
    virtual ~HephaestusObserver() { }
};