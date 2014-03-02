#include "stdafx.h"
#include "NetworkConnection.h"

#include "SelectCommand.h"
BOOST_CLASS_EXPORT(SelectCommand)
#include "MoveCommand.h"
BOOST_CLASS_EXPORT(MoveCommand)
#include "AttackCommand.h"
BOOST_CLASS_EXPORT(AttackCommand)
#include "AttackMoveCommand.h"
BOOST_CLASS_EXPORT(AttackMoveCommand)

const int NetworkConnection::HEADER_SIZE = 4;

NetworkConnection::NetworkConnection(
    boost::asio::ip::tcp::iostream* tcpStream) : socketStream(tcpStream) {
  socket = nullptr;
}

NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket* socket) :
    socket(socket) {
  socketStream = nullptr;
}

void NetworkConnection::AddCommands(CommandTurn* commands) {
  if (socket) {
    std::ostringstream outStream;
    boost::archive::text_oarchive serializer(outStream);
    serializer << const_cast<CommandTurn* const>(commands);
    std::string outString = outStream.str();
    std::ostringstream headerStream;
    headerStream << std::setw(HEADER_SIZE)
      << std::hex << outString.size();

    std::string headerString = headerStream.str();
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(headerString));
    buffers.push_back(boost::asio::buffer(outString));

    boost::system::error_code e;
    boost::asio::write(*socket, buffers, e);

  } else {
    boost::archive::text_oarchive serializer(*socketStream);
    serializer << const_cast<CommandTurn* const>(commands);
    socketStream->flush();
  }
}

void NetworkConnection::AddCommand(Command* command) {
  //boost::archive::text_oarchive serializer(*socketStream);
  //serializer << const_cast<Command* const>(command);
  //socketStream->flush();
}

CommandTurn* NetworkConnection::TakeCommandTurn() {
  CommandTurn *commands = nullptr;
  if (socket) {
    char headerString[HEADER_SIZE];
    bool isWaiting = true;
    std::clock_t startTime = std::clock();
    boost::system::error_code e;
    boost::asio::async_read(*socket, boost::asio::buffer(headerString), 
        [&isWaiting](const boost::system::error_code& e,
                     std::size_t bytes) {
          if (!e) {
            isWaiting = false;
          }
        });

    // TODO: instead of busy waiting, sleep and then wake the thread in the
    // callback.
    while (isWaiting && (clock() - startTime) / CLOCKS_PER_SEC <= 1) { }

    if (isWaiting) { // The read has timed out.
      socket->close();
    } else {
      std::istringstream headerStream(std::string(headerString, HEADER_SIZE));
      std::size_t dataSize = 0;
      headerStream >> std::hex >> dataSize;
      std::vector<char> dataString(dataSize);
      boost::asio::read(*socket, boost::asio::buffer(dataString));
      std::string archiveString(&dataString[0], dataString.size());
      std::istringstream archiveStream(archiveString);
      boost::archive::text_iarchive deserializer(archiveStream);
      deserializer >> commands;
    }
  } else {
    boost::archive::text_iarchive deserializer(*socketStream);
    deserializer >> commands;
  }
  return commands;
}