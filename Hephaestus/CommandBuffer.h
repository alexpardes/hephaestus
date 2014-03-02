#pragma once
#include "CommandSource.h"
#include "CommandSink.h"

// This class should be thread-safe.
class CommandBuffer : public CommandSource, public CommandSink {
  public:
    CommandBuffer();
    virtual CommandTurn* TakeCommandTurn();
    virtual void AddCommand(Command* command);
    virtual void AddCommands(CommandTurn* commands);

    // Registered sinks will be sent the current command turn when
    // TakeCommandTurn is called.
    void RegisterCommandSink(CommandSink* sink);

    void CreateTurnDelay(int nTurns);

  private:
    CommandTurn* buffer;
    std::vector<CommandSink*> sinks;
    std::mutex lock;
};