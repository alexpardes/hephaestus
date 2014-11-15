#include "CommandSource.h"
#include "CommandSink.h"
#include <boost/asio/ip/tcp.hpp>

class NetworkConnection : public CommandSource, public CommandSink {
  public:
    NetworkConnection(boost::asio::ip::tcp::socket* socket);
    virtual std::shared_ptr<CommandTurn> TakeCommandTurn();
    virtual void AddCommand(std::shared_ptr<Command> command);
    virtual void AddCommands(std::shared_ptr<CommandTurn> commands);
    virtual void SetGameHash(size_t gameHash) { } // TODO: Does this make sense?

  private:
    boost::asio::ip::tcp::socket* socket;
    std::shared_ptr<CommandTurn> receivedCommands;
};