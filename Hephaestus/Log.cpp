#include "stdafx.h"
#include <boost/filesystem.hpp>
#include "Log.h"

bool Log::isInitialized = false;
std::ofstream Log::logFile;

void Log::Init() {
  if (isInitialized)
    return;

  boost::filesystem::path gameFolder = boost::filesystem::path(std::getenv("APPDATA")) / "Firefight";
  boost::filesystem::create_directory(gameFolder);
  logFile = std::ofstream(gameFolder.string() + "/log.txt");
  isInitialized = true;
}

void Log::Write(const std::string &message) {
  Init();
  logFile << message << std::endl;
}