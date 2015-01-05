#include "stdafx.h"
#include "CommandTurn.h"
#include "Replay.h"

void ReplayWriter::OpenFile(const std::string &filename,
                            const std::string &map) {
  file.open(filename, std::ios::binary);
  file << map << '\n';
}

void ReplayWriter::AddCommands(std::shared_ptr<CommandTurn> commands) {
  std::array<char, 256> bytes;
  char *bytesStart = &bytes[0];
  char *bytePointer = &bytes[0];
  commands->Serialize(bytePointer);
  size_t size = bytePointer - bytesStart;
  file.write(bytesStart, size);
}

void ReplayWriter::CloseFile() {
  file.close();
}

ReplayReader::ReplayReader() {
  rawReplayStart = nullptr;
}

ReplayReader::ReplayReader(const std::string &filename) {
  OpenFile(filename);
}

ReplayReader::ReplayReader(const std::string &filename, int playerId) : playerId(playerId) {
  OpenFile(filename);
}

ReplayReader::~ReplayReader() {
  if (rawReplayStart)
    delete rawReplayStart;
}

void ReplayReader::WriteHumanReadable(const std::string &filename) {
  std::ofstream outputFile(filename);
  int plyNumber = 0;
  while (size_t(rawReplayCurrent - rawReplayStart) < fileSize) {
    auto turn = CommandTurn::Deserialize(rawReplayCurrent);
    WriteTurn(*turn, plyNumber++, outputFile);
  }

  outputFile.close();
}

void ReplayReader::WriteTurn(const CommandTurn &turn, int plyNumber, std::ofstream &outputFile) {
  std::ostringstream stringStream;
  stringStream << plyNumber / 2 << " Player" << plyNumber % 2 + 1 << ": ";
  std::string prefix = stringStream.str();
  for (auto command : turn) {
    outputFile << prefix << command->ToString() << std::endl;
  }
}

std::shared_ptr<CommandTurn> ReplayReader::TakeCommandTurn() {
  // TODO: Indicate that the replay has finished.
  if (rawReplayCurrent >= rawReplayStart + fileSize)
    return std::make_shared<CommandTurn>();

  auto turn1 = CommandTurn::Deserialize(rawReplayCurrent);
  auto turn2 = CommandTurn::Deserialize(rawReplayCurrent);
  return playerId == 0 ? turn1 : turn2;
}

void ReplayReader::SetGameHash(size_t gameHash) { }

size_t Find(const char *dataStart, size_t size, char search) {
  for (size_t pos = 0; pos < size; ++pos) {
    if (dataStart[pos] == search)
      return pos;
  }
  return size;
}

void ReplayReader::OpenFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary);
  file.seekg(0, std::ios::end);
  fileSize = (size_t) file.tellg();
  file.seekg(0, std::ios::beg);
  rawReplayStart = new char[fileSize];
  rawReplayCurrent = rawReplayStart;
  file.read(rawReplayStart, fileSize);
  rawReplayCurrent = rawReplayStart + Find(rawReplayStart, fileSize, '\n') + 1;
}