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

ReplayReader::ReplayReader() { }

ReplayReader::ReplayReader(const std::string &filename) {
  OpenFile(filename);
}

void ReplayReader::OpenFile(const std::string &filename) {
  file.open(filename, std::ios::binary);
}

void ReplayReader::WriteHumanReadable(const std::string &filename) {
  std::filebuf* fileBuffer = file.rdbuf();
  auto fileSize = fileBuffer->pubseekoff(0, file.end, file.in);
  fileBuffer->pubseekpos(0, file.in);
  char *bytesStart = new char[(size_t) fileSize];
  fileBuffer->sgetn(bytesStart, fileSize);

  std::ofstream outputFile(filename);
  char *bytes = bytesStart;

  int plyNumber = 0;
  while (bytes - bytesStart < fileSize) {
    auto turn = CommandTurn::Deserialize(bytes);
    WriteTurn(*turn, plyNumber++, outputFile);
  }

  delete[] bytesStart;
}

void ReplayReader::WriteTurn(const CommandTurn &turn, int plyNumber, std::ofstream &outputFile) {
  std::ostringstream stringStream;
  stringStream << plyNumber / 2 << " Player" << plyNumber % 2 + 1 << ": ";
  std::string prefix = stringStream.str();
  for (auto command : turn) {
    outputFile << prefix << command->ToString() << std::endl;
  }
}