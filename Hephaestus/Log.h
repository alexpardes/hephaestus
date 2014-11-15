#pragma once
#include <iostream>

class Log {
  public:
    static void Write(const std::string &message);

  private:
    static std::ofstream logFile;
};