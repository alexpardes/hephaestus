#include "GridRegion.h"

GridRegion::GridRegion(const Vector2i &point,
                       const Vector2i &dir1,
                       const Vector2i &dir2) {
  offset = -point;
  direction1 = dir1;
  direction2 = dir2;
  isEmpty = false;

  containedSide1 = Compare(dir1, dir2);
  containedSide2 = Compare(dir2, dir1);
}

// Returns 0 if point is on the line from the origin in input direction.
// Returns 1 for points on one side of this line, and -1 for points
// on the other side.
int GridRegion::Compare(const Vector2i &direction, const Vector2i &point) {
  int comparison =  direction.x * point.y - direction.y * point.x;
  int result = 0;
  if (comparison > 0) {
    result = 1;
  } else if (comparison < 0) {
    result = -1;
  }
  return result;
}

bool GridRegion::Contains(const Vector2i &point) {
  if (isEmpty) {
    return false;
  }

  // Brings p to the base of the region.
  Vector2i p = point + offset;
  int side1 = Compare(direction1, p);
  int side2 = Compare(direction2, p);

  bool sameSide1 = side1 == 0 || side1 == containedSide1;		
  bool sameSide2 = side2 == 0 || side2 == containedSide2;

  return sameSide1 && sameSide2;
}

GridRegion GridRegion::GetIndirect(const Vector2i &p, const Vector2i &q) {
  if (p == q) {
    return GridRegion();
  }

  Vector2i dir1, dir2;

  Vector2i point(q.x - p.x, q.y - p.y);
  int a = Compare(Vector2i(1, 0), point);
  int b = Compare(Vector2i(0, -1), point);
  if (a == 0) {
    dir1 = Vector2i(b, -1);
    dir2 = Vector2i(b, 1);
  } else if (b == 0) {
    dir1 = Vector2i(-1, a);
    dir2 = Vector2i(1, a);
  } else {
    int c = Compare(Vector2i(b, a), point);
    if (c == 0) {
      dir1 = Vector2i(0, a);
      dir2 = Vector2i(b, 0);
    } else {
      dir1 = Vector2i(b, a);
      if (a*b*c == 1) {
        dir2 = Vector2i(0, a);
      } else {
        dir2 = Vector2i(b, 0);
      }
    }
  }

  return GridRegion(q, dir1, dir2);
}