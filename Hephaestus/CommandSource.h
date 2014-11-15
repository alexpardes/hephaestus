#pragma once
#include <memory>

class CommandTurn;

class CommandSource {
  public:
    virtual std::shared_ptr<CommandTurn> TakeCommandTurn() = 0;
    virtual void SetGameHash(size_t gameHash) = 0;
    virtual ~CommandSource() { }
    static CommandSource *Null;

  private:
    class NullCommandSource;
};

class CommandSource::NullCommandSource : public CommandSource {
  public:
    virtual std::shared_ptr<CommandTurn> TakeCommandTurn();
    virtual void SetGameHash(size_t gameHash);

  private:
    size_t gameHash;
};