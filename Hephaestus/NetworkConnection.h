#include "CommandSource.h"
#include "CommandSink.h"
#include <boost/asio/ip/tcp.hpp>

class NetworkConnection : public CommandSource, public CommandSink {
  public:
    NetworkConnection(boost::asio::ip::tcp::iostream* tcpStream);
    NetworkConnection(boost::asio::ip::tcp::socket* socket);
    virtual std::shared_ptr<CommandTurn> TakeCommandTurn();
    virtual void AddCommand(std::shared_ptr<Command> command);
    virtual void AddCommands(std::shared_ptr<CommandTurn> commands);

  private:
    void DeserializeCommands();
    boost::asio::ip::tcp::iostream* socketStream;
    boost::asio::ip::tcp::socket* socket;
    std::shared_ptr<CommandTurn> receivedCommands;
    const static int HEADER_SIZE;
};