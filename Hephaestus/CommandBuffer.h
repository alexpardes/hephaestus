#pragma once
#include "CommandSource.h"
#include "CommandSink.h"

// This class should be thread-safe.
class CommandBuffer : public CommandSource, public CommandSink {
  public:
    CommandBuffer();
    virtual std::shared_ptr<CommandTurn> TakeCommandTurn();
    virtual void AddCommand(std::shared_ptr<Command> command);
    virtual void AddCommands(std::shared_ptr<CommandTurn> commands);
    virtual void SetGameHash(size_t gameHash);

    // Registered sinks will be sent the current command turn when
    // TakeCommandTurn is called.
    void RegisterCommandSink(CommandSink* sink);
    void CreateTurnDelay(int nTurns);

  private:
    void SendCommands(std::shared_ptr<CommandTurn> commands) const;
    void EmptyBuffer();

    std::shared_ptr<CommandTurn> bufferTurn;
    std::queue<std::shared_ptr<CommandTurn>> turnQueue;
    std::vector<CommandSink*> sinks;
    std::mutex lock;
};