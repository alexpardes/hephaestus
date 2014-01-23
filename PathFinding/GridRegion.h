#pragma once
#include <Hephaestus/PathFinder.h>

class GridRegion {
  public:
    GridRegion(const Vector2i &point, const Vector2i &dir1, const Vector2i &dir2);
    bool Contains(const Vector2i &point);

    // Creates the region of points r such that there is an h-path
    // from p to r passing through q. 
    static GridRegion GetIndirect(const Vector2i &p, const Vector2i &q);

  protected:
    GridRegion() { isEmpty = true; }

  private:
    static int Compare(const Vector2i &direction, const Vector2i &point);
    Vector2i offset, direction1, direction2;
    int containedSide1, containedSide2;
    bool isEmpty;
};