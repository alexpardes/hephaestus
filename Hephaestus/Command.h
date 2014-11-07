#pragma once
#include "Util.h"
#include "VectorSerialization.h"
#include "GameState.h"
#include "Serialization.h"

class Player;

typedef unsigned char CommandType;

class Command {
  public:
    typedef std::shared_ptr<Command> (*Deserializer)(ByteIterator start, ByteIterator end);
    static std::shared_ptr<Command> Deserialize(CommandType type, ByteIterator start, ByteIterator end);
    static CommandType Register(Deserializer deserializer);

		virtual void Execute(Player &player) const = 0;
    virtual std::vector<unsigned char> Serialize() const = 0;

    // This method must return the value given by Register.
    virtual CommandType Type() const = 0;
    virtual ~Command() { }

  private:
    static std::vector<Deserializer> deserializers;
};
typedef std::vector<const std::shared_ptr<const Command>> CommandTurn;