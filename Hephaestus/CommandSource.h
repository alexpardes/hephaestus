#pragma once
#include "Command.h"

class CommandSource {
  public:
    virtual CommandTurn* TakeCommandTurn() = 0;
    virtual ~CommandSource() { }
    static CommandSource *Null;

  private:
    class NullCommandSource;
};

class CommandSource::NullCommandSource : public CommandSource {
  public:
    virtual CommandTurn* TakeCommandTurn();
};