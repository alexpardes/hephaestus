#pragma once
#include <iostream>
#include <string>
#include "CommandSink.h"
#include "CommandSource.h"

class ReplayWriter : public CommandSink {
  public:
    void OpenFile(const std::string &filename, const std::string &map);
    virtual void AddCommands(std::shared_ptr<CommandTurn> commands);
    virtual void AddCommand(std::shared_ptr<Command> command) { }
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