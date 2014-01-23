#ifndef COMMAND_
#define COMMAND_

#include "Util.h"
#include "Players.h"
#include "VectorSerialization.h"
#include "GameState.h"

class Simulator;

class Command {
  public:
		virtual void Execute(Player &player) const = 0;
    virtual ~Command() { }

  private:
    friend class boost::serialization::access;
		template<class Archive>
    void serialize(Archive & ar, const unsigned int version) { }
};
typedef std::vector<const Command*> CommandTurn;

#endif