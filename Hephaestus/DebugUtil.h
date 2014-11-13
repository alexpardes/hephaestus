#pragma once
#include <string>

class DebugUtil {
  public:
    static void Write(const std::string &message);

  private:
    static std::string logFile;
};