#pragma once
#include "Command.h"

class CommandTurn {
  public:
    typedef std::vector<const std::shared_ptr<const Command>>::iterator iterator;
    typedef std::vector<const std::shared_ptr<const Command>>::const_iterator const_iterator;

    CommandTurn() : hash(0) { }
    CommandTurn(size_t gameHash) : hash(gameHash) { }

    iterator begin() { return commands.begin(); }
    const_iterator begin() const { return commands.begin(); }
    iterator end() { return commands.end(); }
    const_iterator end() const { return commands.end(); }
    size_t GameHash() const { return hash; }

    void Add(const std::shared_ptr<const Command> command) { commands.push_back(command); }
    void SetHash(size_t hash) { this->hash = hash; }
      
    // Returns with bytes pointing to the byte after the end of the CommandTurn.
    void Serialize(char *&bytes) const;
    static std::shared_ptr<CommandTurn> Deserialize(char *&bytes);

  private:
    size_t hash;
    std::vector<const std::shared_ptr<const Command>> commands;
};