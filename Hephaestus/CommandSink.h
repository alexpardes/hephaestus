#pragma once
#include "Command.h"

class CommandSink {
  public:
    virtual void AddCommand(std::shared_ptr<Command> command) = 0;
    virtual void AddCommands(std::shared_ptr<CommandTurn> commands) = 0;
    virtual ~CommandSink() { }
    static CommandSink *Null;

  private:
    class NullCommandSink;
};

class CommandSink::NullCommandSink : public CommandSink {
  public:
    virtual void AddCommand(std::shared_ptr<Command> command) { }
    virtual void AddCommands(std::shared_ptr<CommandTurn> commands) { }
};