#include "stdafx.h"
#include "CommandTurn.h"
#include "NetworkConnection.h"

const size_t kMaxMessageSize = 256;

NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket* socket) :
  socket(socket) { }

void NetworkConnection::AddCommands(std::shared_ptr<CommandTurn> commands) {
  std::array<char, kMaxMessageSize> message;
  char* bytes = &message[0];
  commands->Serialize(bytes);
  boost::asio::write(*socket, boost::asio::buffer(message));
}

void NetworkConnection::AddCommand(std::shared_ptr<Command> command) { }

std::shared_ptr<CommandTurn> NetworkConnection::TakeCommandTurn() {
  std::array<char, kMaxMessageSize> message;
  bool isWaiting = true;
  std::clock_t startTime = std::clock();
  boost::system::error_code e;
  boost::asio::async_read(*socket, boost::asio::buffer(message), 
      [&isWaiting](const boost::system::error_code& e,
                    std::size_t bytes) {
        if (!e) {
          isWaiting = false;
        }
      });

  // TODO: instead of busy waiting, sleep and then wake the thread in the
  // callback.
  while (isWaiting && (clock() - startTime) / CLOCKS_PER_SEC <= 2);

  if (isWaiting) { // The read has timed out.
    socket->close();
  } else {
    char* bytes = &message[0];
    return CommandTurn::Deserialize(bytes);
  }

  return nullptr;
}