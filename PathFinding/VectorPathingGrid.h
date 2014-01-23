#pragma once
#include "Hephaestus/PathFinder.h"

class VectorPathingGrid : public PathingGrid {
  public:
    VectorPathingGrid(const Vector2i &size);
    VectorPathingGrid(int width, int height);
    virtual bool IsBlocked(const Vector2i &tile) const;
    virtual bool IsUnblocked(const Vector2i &tile) const;
    virtual void SetBlocked(const Vector2i &tile, bool isBlocked);
    virtual Vector2i Size() const;

  private:
    void Construct(const Vector2i &size);
    bool Contains(const Vector2i &tile) const;
    std::vector<std::vector<bool>> isBlocked;
};