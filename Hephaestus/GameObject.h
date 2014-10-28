#pragma once
#include "Vector2.h"

class GameObject {
  public:
    virtual void PerformAction() = 0;
    virtual ~GameObject() { }
    Vector2f Position() const { return position; }
    void SetPosition(const Vector2f &position) { this->position = position; }
    float Rotation() const { return rotation_; }
    void SetRotation(float rotation) { rotation_ = rotation; }

  protected:
    Vector2f position;
    float rotation_;
};