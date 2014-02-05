#pragma once
#include <string>

class UnitAction;

class UnitAbility {
  public:
    virtual void Execute() = 0;
    virtual std::string Name() = 0;
    virtual ~UnitAbility() { }
};