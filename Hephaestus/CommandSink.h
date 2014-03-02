#pragma once
#include "Command.h"

class CommandSink {
  public:
    virtual void AddCommand(Command* command) = 0;
    virtual void AddCommands(CommandTurn* commands) = 0;
    virtual ~CommandSink() { }
    static CommandSink *Null;

  private:
    class NullCommandSink;
};

class CommandSink::NullCommandSink : public CommandSink {
  public:
    virtual void AddCommand(Command* command) { }
    virtual void AddCommands(CommandTurn* commands) { }
};