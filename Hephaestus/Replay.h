#pragma once
#include <iostream>
#include <string>
#include "CommandSource.h"

class ReplayWriter {
  public:
    void OpenFile(const std::string &filename);
    void WriteTurn(const CommandTurn &turn);
    void CloseFile();

  private:
    std::ofstream file;
};

class ReplayReader : public CommandSource {
  public:
    ReplayReader();
    ReplayReader(const std::string &filename);
    ReplayReader(const std::string &filename, int playerId);
    ~ReplayReader();
    void OpenFile(const std::string &filename);
    void WriteHumanReadable(const std::string &filename);

    virtual std::shared_ptr<CommandTurn> TakeCommandTurn();
    virtual void SetGameHash(size_t gameHash);

  private:
    void WriteTurn(const CommandTurn &turn, int plyNumber, std::ofstream &outputFile);

    int playerId;
    char *rawReplayStart;
    char *rawReplayCurrent;
    size_t fileSize;
};