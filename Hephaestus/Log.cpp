#include "stdafx.h"
#include "Log.h"

std::ofstream Log::logFile("log.txt");

void Log::Write(const std::string &message) {
  logFile << message << std::endl;
}