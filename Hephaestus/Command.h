#pragma once

#include "Util.h"
#include "VectorSerialization.h"
#include "GameState.h"

class Player;

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