#pragma once
#include <string>

class UnitAbility {
  public:
    virtual void Execute() { };
    virtual void ExecutePassive() { }
    virtual std::string Name() const = 0;
    virtual ~UnitAbility() { }
};