#pragma once
#include "Vector2.h"

class GameObject {
  public:
    virtual void PerformAction() = 0;
    virtual ~GameObject() { }
    Vector2f Position() const { return position_; }
    void SetPosition(const Vector2f &position) { position_ = position; }
    float Rotation() const { return rotation_; }
    void SetRotation(float rotation) { rotation_ = rotation; }

  protected:
    Vector2f position_;
    float rotation_;
};