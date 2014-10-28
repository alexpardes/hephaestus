#pragma once
#include "PathFinder.h"

class VectorPathingGrid : public PathingGrid {
  public:
    VectorPathingGrid(const Vector2i &size);
    VectorPathingGrid(int width, int height);

    using PathingGrid::IsBlocked;
    using PathingGrid::IsUnblocked;
    using PathingGrid::SetBlocked;
    bool IsBlocked(const Vector2i &tile) const;
    bool IsUnblocked(const Vector2i &tile) const;
    void SetBlocked(const Vector2i &tile, bool isBlocked);
    virtual Vector2i Size() const;

  private:
    void Construct(const Vector2i &size);
    bool Contains(const Vector2i &tile) const;
    std::vector<std::vector<bool>> isBlocked;
};