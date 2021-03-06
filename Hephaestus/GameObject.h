#pragma once
#include "Util.h"
#include "Vector2.h"

class GameObject {
  public:
    virtual size_t HashCode() const {
      size_t hash = Util::HashStart();
      Util::Hash(hash, position);
      Util::Hash(hash, rotation);
      return hash;
    }

    virtual void PerformAction() = 0;
    virtual ~GameObject() { }
    Vector2f Position() const { return position; }
    void SetPosition(const Vector2f &position) { this->position = position; }
    float Rotation() const { return rotation; }
    void SetRotation(float rotation) { this->rotation = Util::Angle(rotation); }

  protected:
    Vector2f position;
    float rotation;
};