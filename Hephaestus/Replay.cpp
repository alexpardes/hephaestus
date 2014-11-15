#include "stdafx.h"
#include "CommandTurn.h"
#include "Replay.h"

void ReplayWriter::OpenFile(const std::string &filename) {
  file.open(filename, std::ios::binary);
}

void ReplayWriter::WriteTurn(const CommandTurn &turn) {
  std::array<char, 256> bytes;
  char *bytesStart = &bytes[0];
  char *bytePointer = &bytes[0];
  turn.Serialize(bytePointer);
  file.write(bytesStart, bytePointer - bytesStart);
}

void ReplayWriter::CloseFile() {
  file.close();
}

ReplayReader::ReplayReader() { }

ReplayReader::ReplayReader(const std::string &filename) {
  OpenFile(filename);
}

void ReplayReader::OpenFile(const std::string &filename) {
  file.open(filename, std::ios::binary);
}

void ReplayReader::WriteHumanReadable(const std::string &filename) {
  file.seekg(0, std::ios::end);
  auto fileSize = (size_t) file.tellg();
  file.seekg(0, std::ios::beg);
  char *bytesStart = new char[fileSize];
  file.read(bytesStart, fileSize);
  std::ofstream outputFile(filename);
  char *bytes = bytesStart;
  int plyNumber = 0;
  while (size_t(bytes - bytesStart) < fileSize) {
    auto turn = CommandTurn::Deserialize(bytes);
    WriteTurn(*turn, plyNumber++, outputFile);
  }

  delete[] bytesStart;
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