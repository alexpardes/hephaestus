#pragma once
#include "UnitAction.h"
#include "Vector2.h"

class TargetGroundAbility;

class TargetGroundAction : public UnitAction {
  public:
    TargetGroundAction(const Vector2f &location, bool fullCover);
    virtual void Start(GameUnit &unit);
    virtual void Execute(GameUnit &unit);
    virtual bool IsFinished() const;
    virtual std::string Name() const;

private:
  Vector2f location;
  TargetGroundAbility *ability;
  bool fullCover;
};