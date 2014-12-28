#include "stdafx.h"
#include "CommandTurn.h"
#include "NetworkConnection.h"
#include "Serialization.h"

const size_t kMaxMessageSize = 256;

NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket* socket) :
  socket(socket) { }

#define MAX_STRING_SIZE 128

void NetworkConnection::Send(const std::string &message) {
  std::array<char, MAX_STRING_SIZE> buffer;
  size_t bufferIndex = 0;
  buffer[bufferIndex++] = message.length();
  for (auto character : message) {
    buffer[bufferIndex++] = character;
  }
  boost::asio::write(*socket, boost::asio::buffer(buffer));
}

std::string NetworkConnection::Read() {
  std::array<char, MAX_STRING_SIZE> buffer;
  boost::system::error_code e;
  boost::asio::read(*socket, boost::asio::buffer(buffer), boost::asio::transfer_all(), e);
  std::stringstream stringStream;
  auto length = buffer[0];
  for (int i = 1; i <= length; ++i) {
    stringStream << buffer[i];
  }
  return stringStream.str();
}

void NetworkConnection::Send(int message) {
  std::array<char, 4> buffer;
  char *bytes = &buffer[0];
  Serialization::Write(bytes, message);
  boost::asio::write(*socket, boost::asio::buffer(buffer));
}

int NetworkConnection::ReadInt() {
  std::array<char, 4> message;
  boost::system::error_code e;
  boost::asio::read(*socket, boost::asio::buffer(message), e);
  char *bytes = &message[0];
  return Serialization::Read<int>(bytes);
}


float NetworkConnection::Ping() {
  std::array<char, 1> buffer;
  char message = '\n';
  boost::system::error_code e;

  // The first message received tells us that the client is ready.
  boost::asio::read(*socket, boost::asio::buffer(buffer), e);
  auto time = std::clock();
  boost::asio::write(*socket, boost::asio::buffer(&message, sizeof(message)), e);
  boost::asio::read(*socket, boost::asio::buffer(buffer), e);
  return (std::clock() - time) * 1000.f / CLOCKS_PER_SEC;
}

void NetworkConnection::AwaitPing() {
  char message = '\n';
  boost::system::error_code e;
  boost::asio::write(*socket, boost::asio::buffer(&message, sizeof(message)), e);
  std::array<char, 1> buffer;
  boost::asio::read(*socket, boost::asio::buffer(buffer), e);
  boost::asio::write(*socket, boost::asio::buffer(&message, sizeof(message)), e);
}

void NetworkConnection::AddCommands(std::shared_ptr<CommandTurn> commands) {
  std::array<char, kMaxMessageSize> message;
  char *bytes = &message[0];
  commands->Serialize(bytes);
  boost::system::error_code ec;
  boost::asio::write(*socket, boost::asio::buffer(message), ec);
}

void NetworkConnection::AddCommand(std::shared_ptr<Command> command) { }

std::shared_ptr<CommandTurn> NetworkConnection::TakeCommandTurn() {
  std::array<char, kMaxMessageSize> message;
  volatile bool isWaiting = true;
  std::clock_t startTime = std::clock();
  boost::system::error_code e;
  boost::asio::async_read(*socket, boost::asio::buffer(message), 
      [&isWaiting](const boost::system::error_code& e,
                    std::size_t bytes) {
        if (!e)
          isWaiting = false;
      });

#if _DEBUG
  while (isWaiting && (clock() - startTime) / CLOCKS_PER_SEC <= 3600);
#else
  while (isWaiting && (clock() - startTime) / CLOCKS_PER_SEC <= 1);
#endif

  if (isWaiting) { // The read has timed out.
    socket->close();
  } else {
    char* bytes = &message[0];
    return CommandTurn::Deserialize(bytes);
  }

  return nullptr;
}