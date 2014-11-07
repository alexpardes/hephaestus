#include "stdafx.h"
#include "NetworkConnection.h"

/* 
 * Each packet begins with a 1 byte giving the size of the remainder of the packet.
 * The rest of the packet is a list of commands.
 * Each begins with one byte specifying the type of command and one byte giving the size of the remainder of the command.
 * The rest can be deserialized by the appropriate function.
 */

const size_t MAX_MESSAGE_SIZE = 256;

NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket* socket) :
  socket(socket) { }

void NetworkConnection::AddCommands(std::shared_ptr<CommandTurn> commands) {
  unsigned char messageSize = 1;
  std::array<unsigned char, MAX_MESSAGE_SIZE> data;

  for (auto command : *commands) {
    unsigned char commandType = command->Type();
    data[messageSize++] = commandType;
    auto bytes = command->Serialize();
    data[messageSize++] = bytes.size();
    std::copy(bytes.begin(), bytes.end(), &data[messageSize]);
    messageSize += bytes.size();
  }
  data[0] = messageSize - 1;
  boost::asio::write(*socket, boost::asio::buffer(data, messageSize));
}

void NetworkConnection::AddCommand(std::shared_ptr<Command> command) {
  //boost::archive::text_oarchive serializer(*socketStream);
  //serializer << const_cast<Command* const>(command);
  //socketStream->flush();
}

std::shared_ptr<CommandTurn> NetworkConnection::TakeCommandTurn() {
  auto commandTurn = std::make_shared<CommandTurn>();
  char messageSize;
  bool isWaiting = true;
  std::clock_t startTime = std::clock();
  boost::system::error_code e;
  OutputDebugStringA("async_read\n");
  boost::asio::async_read(*socket, boost::asio::buffer(&messageSize, 1), 
      [&isWaiting](const boost::system::error_code& e,
                    std::size_t bytes) {
        if (!e) {
          OutputDebugStringA("async_read handler\n");
          isWaiting = false;
        } else {
          OutputDebugStringA("async_read handler error\n");
          OutputDebugStringA(e.message().c_str());
        }
      });

  // TODO: instead of busy waiting, sleep and then wake the thread in the
  // callback.
  while (isWaiting && (clock() - startTime) / CLOCKS_PER_SEC <= 2);
  OutputDebugStringA("Done waiting!");

  if (isWaiting) { // The read has timed out.
    socket->close();
  } else {
    std::vector<unsigned char> message(messageSize);
    boost::system::error_code ec;
    boost::asio::read(*socket, boost::asio::buffer(message), ec);

    char iBlockStart = 0;
    while (iBlockStart < messageSize) {
      CommandType commandType = message[iBlockStart++];
      unsigned char commandSize = message[iBlockStart++];
      auto dataBegin = message.begin() + iBlockStart;
      auto dataEnd = dataBegin + commandSize;
      commandTurn->push_back(Command::Deserialize(commandType, dataBegin, dataEnd));
      iBlockStart += commandSize;
    }
  }

  return commandTurn;
}