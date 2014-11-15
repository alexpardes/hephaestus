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

class ReplayReader {
  public:
    ReplayReader();
    ReplayReader(const std::string &filename);
    void OpenFile(const std::string &filename);
    void WriteHumanReadable(const std::string &filename);

  private:
    void WriteTurn(const CommandTurn &turn, int plyNumber, std::ofstream &outputFile);

    std::ifstream file;
};