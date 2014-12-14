#pragma once
#include <iostream>

class Log {
  public:
    static void Write(const std::string &message);

  private:
    static void Init();

    static std::ofstream logFile;
    static bool isInitialized;
};