#ifndef COMMAND_
#define COMMAND_

#include "GameUnit.h"
#include "Util.h"
#include "VectorSerialization.h"

class Command {
        public:
                Command() { }
				enum CommandType { kAttack, kAttackMove, kMove, kSelect };
				virtual ~Command() { };
				virtual CommandType type() const = 0;
				virtual void Execute(const std::vector<GameUnit> *units) = 0;

        private:
                friend class boost::serialization::access;
				template<class Archive>
                void serialize(Archive & ar, const unsigned int version) { }        
};
typedef std::vector<const Command*> CommandTurn;

#endif