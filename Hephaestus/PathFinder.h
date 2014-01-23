#pragma once
#include "Util.h"

class PathingGrid {
  public:
    virtual bool IsBlocked(const Vector2i &tile) const = 0;
    bool IsBlocked(int x, int y) const { return IsBlocked(Vector2i(x, y)); }
    virtual bool IsUnblocked(const Vector2i &tile) const = 0;
    bool IsUnblocked(int x, int y) const { return IsUnblocked(Vector2i(x, y)); }
    virtual void SetBlocked(const Vector2i &tile, bool isBlocked) = 0;
    void SetBlocked(const Vector2i &tile) { SetBlocked(tile, true); }
    void SetBlocked(int x, int y, bool isBlocked) { SetBlocked(Vector2i(x, y), isBlocked); }
    void SetBlocked(int x, int y) { SetBlocked(Vector2i(x, y)); }
    virtual Vector2i Size() const = 0;
    virtual ~PathingGrid() { }
    static float OctileDistance(const Vector2i &p1, const Vector2i &p2);
};

class PathFinder {
  public:
    virtual std::vector<Vector2i> GetPath(const Vector2i &start,
        const Vector2i &end) = 0;
    virtual ~PathFinder() { }
};