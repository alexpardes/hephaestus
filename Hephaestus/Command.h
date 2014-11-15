#pragma once
#include <string>
#include "Util.h"
#include "VectorSerialization.h"
#include "GameState.h"

class Player;

typedef char CommandType;

class Command {
  public:
    typedef std::function<std::shared_ptr<Command>(char*&)> Deserializer;
    static std::shared_ptr<Command> Deserialize(char *&bytes);
    static CommandType Register(Deserializer deserializer);

		virtual void Execute(Player &player) const = 0;
    virtual std::string ToString() const = 0;
    void Serialize(char *&bytes) const;

    virtual ~Command() { }

  protected:
    // This method must return the value given by Register.
    virtual CommandType Type() const = 0;
    virtual void SerializeInternal(char *&bytes) const = 0;

  private:
    static std::vector<Deserializer> deserializers;
};