#include "CommandSource.h"
#include "CommandSink.h"
#include <boost/asio/ip/tcp.hpp>

class NetworkConnection : public CommandSource, public CommandSink {
  public:
    NetworkConnection(boost::asio::ip::tcp::iostream* tcpStream);
    NetworkConnection(boost::asio::ip::tcp::socket* socket);
    virtual CommandTurn* TakeCommandTurn();
    virtual void AddCommand(Command* command);
    virtual void AddCommands(CommandTurn* commands);

  private:
    void DeserializeCommands();
    boost::asio::ip::tcp::iostream* socketStream;
    boost::asio::ip::tcp::socket* socket;
    CommandTurn* receivedCommands;
    const static int HEADER_SIZE;
};