#include "stdafx.h"
#include "ShadowCaster.h"
#include "Util.h"
#include <PathFinding/Pathfinder.h>

std::vector<float> ShadowCaster::FindSectorIntersection(float s1,
                                                        float e1,
                                                        float s2,
                                                        float e2) {
  std::vector<float> result;
  float intersectionStart;
  if (Util::IsBetweenAngles(s1, s2, e2)) {
    intersectionStart = s1;
  } else if (Util::IsBetweenAngles(s2, s1, e1)) {
    intersectionStart = s2;
  } else {
    return result;
  }

  float intersectionEnd;
  if (Util::IsBetweenAngles(e2, intersectionStart, e1)) {
    intersectionEnd = e2;
  } else {
    intersectionEnd = e1;
  }

  result.push_back(intersectionStart);
  result.push_back(intersectionEnd);
  return result;
}

std::vector<Vector2f> ShadowCaster::ShadowCast(const Vector2f& origin,
                                               float startAngle,
                                               float endAngle,
                                               float distance) {
  //return ShadowCast(origin, startAngle, endAngle, distance,
  //    OriginTile(origin).x, false, false);
  
  std::vector<Vector2f> result;
  sectorsToCheck.clear();
  visiblePoints.clear();

  int column = Tile(origin).x;

  for (int i = 0; i < 4; ++i) {
    auto sector = FindSectorIntersection(startAngle, endAngle, i * M_PI/2,
        (i + 1) * M_PI/2);
    if (sector.size()) {
      if (i % 2 != 0) std::swap(sector[0], sector[1]);
      bool flipX = i == 1 || i == 2;
      bool flipY = i == 2 || i == 3;
      QueueColumnSector(column, sector[0], sector[1], flipX, flipY);
    }
  }

  while (!sectorsToCheck.empty()) {
    ColumnSector sector = sectorsToCheck.front();
    sectorsToCheck.pop_front();
    if (sector.startAngle == sector.endAngle)
      continue;

    ShadowCast(origin, sector.startAngle, sector.endAngle, distance,
        sector.column, sector.flipX, sector.flipY);
  }

  return visiblePoints;
}

void ShadowCaster::QueueColumnSector(int column,
                                     float startAngle,
                                     float endAngle,
                                     bool flipX,
                                     bool flipY) {
  ColumnSector sector;
  sector.column = column;
  sector.startAngle = startAngle;
  sector.endAngle = endAngle;
  sector.flipX = flipX;
  sector.flipY = flipY;
  sectorsToCheck.push_back(sector);
}


// TODO: Clean up this method.
// The sector between start and end angles should lie in a single quadrant.
// End angle should be greater than start angle in 1st and 3rd quadrants,
// and less in 2nd and 4th. Set flipX if the quadrant has negative X and flipY
// for negative y.
void ShadowCaster::ShadowCast(Vector2f origin,
                                               float startAngle,
                                               float endAngle,
                                               float distance,
                                               int column,
                                               bool flipX,
                                               bool flipY) {
  int xDir = flipX ? -1 : 1;
  int yDir = flipY ? -1 : 1;

  // Doing a reflection swaps clockwise and counterclockwise.
  bool flipRotation = flipX ^ flipY;

  int startRow, endRow;
  if (!flipY) {
    startRow = FindLowRow(origin, startAngle, distance, column, flipX);
    endRow = FindHighRow(origin, endAngle, distance, column, flipX);
  } else {
    startRow = FindHighRow(origin, startAngle, distance, column, flipX);
    endRow = FindLowRow(origin, endAngle, distance, column, flipX);
  }

  int nextRow = startRow;
  float currentStartAngle = startAngle;

  // Loops through each contiguous set of walls in the column.
  while (yDir * (nextRow - endRow) <= 0) {
    int wallStart = FindFirstWall(column, nextRow, endRow, flipY);
    if (yDir * (wallStart - endRow) <= 0) {
      nextRow = FindFirstOpen(column, wallStart, endRow, flipY);

      int wallEnd = nextRow - yDir;
      auto corners = GetVisibleCorners(column, column, wallStart, wallEnd, origin);
      if (flipRotation) std::reverse(corners.begin(), corners.end());

      float wallStartAngle = Util::FindAngle(corners[0] - origin);

      // Recurses on the space above this wall, unless the top of the wall is
      // above the top vector.
      if (Util::IsBetweenAnglesClosed(wallStartAngle, currentStartAngle, endAngle, flipRotation)) {
        QueueColumnSector(column + xDir, currentStartAngle, wallStartAngle, flipX, flipY);
      }

      if (wallStartAngle == endAngle)
        break;

      // This will store the index of the first corner past the start vector.
      int cornerAfterStart = corners.size() - 1;

      // The last corner must come after the start vector, so we don't need to
      // check it. Other corners are after the start vector if they are between
      // the start vector and the vector to the last corner.
      float lastCornerAngle = Util::FindAngle(corners.back() - origin);
      for (size_t i = 0; i < corners.size() - 1; ++i) {
        float angle = Util::FindAngle(corners[i] - origin);

        if (Util::IsBetweenAngles(angle, currentStartAngle, lastCornerAngle, flipRotation)) {
          cornerAfterStart = i;
          break;
        }
      }

      int cornerBeforeEnd = 0;
      float firstCornerAngle = Util::FindAngle(corners.front() - origin);
      for (int i = corners.size() - 1; i > 0; --i) {
        float angle = Util::FindAngle(corners[i] - origin);
        if (Util::IsBetweenAngles(angle, firstCornerAngle, endAngle, flipRotation)) {
          cornerBeforeEnd = i;
          break;
        }
      }

      std::vector<Vector2f> points;
      if (cornerAfterStart != 0) {
        points.push_back(FindIntersection(origin, currentStartAngle,
            corners[cornerAfterStart - 1], corners[cornerAfterStart]));
      }

      for (int i = cornerAfterStart; i <= cornerBeforeEnd; ++i) {
        points.push_back(corners[i]);
      }

      if (cornerBeforeEnd != corners.size() - 1) {
        points.push_back(FindIntersection(origin, endAngle,
            corners[cornerBeforeEnd], corners[cornerBeforeEnd + 1]));
      } 

      // Adds the visible points to the result.
      for (size_t i = 0; i < points.size() - 1; ++i) {
        visiblePoints.push_back(points[i]);
        visiblePoints.push_back(points[i + 1]);
      }

      float wallEndAngle = Util::FindAngle(points.back() - origin);

      // In this case, this wall occludes any remaining rows in this column.
      if (cornerBeforeEnd != corners.size() - 1 || wallEndAngle == endAngle) {
        break;
      }
      
      currentStartAngle = wallEndAngle;
    } else {
      // Queues the area after the last obstacle.
      QueueColumnSector(column + xDir, currentStartAngle, endAngle, flipX, flipY);
      break;
    }
  }
}

Vector2f ShadowCaster::FindRowIntersection(const Vector2f& origin,
                                           float angle,
                                           int row) const {
    float d = row * tileSize - origin.y;
    return Vector2f(d * std::tanf(M_PI / 2 - angle) + origin.x, row * tileSize);
}

Vector2f ShadowCaster::FindColumnIntersection(const Vector2f& origin,
                                           float angle,
                                           int column) const {
    assert(std::cos(angle) != 0);
    float d = column * tileSize - origin.x;
    return Vector2f(column * tileSize, d * std::tanf(angle) + origin.y);
}

Vector2f ShadowCaster::FindIntersection(const Vector2f& origin,
                                        float angle,
                                        const Vector2f& p1,
                                        const Vector2f& p2) const {
  if (p1.x == p2.x) {
    return FindColumnIntersection(origin, angle, int(p1.x / tileSize));
  } else {
    return FindRowIntersection(origin, angle, int(p1.y / tileSize));
  }
}

// TODO: Shorten this function.
std::vector<Vector2f> ShadowCaster::GetVisibleCorners(int xStart,
                                                      int xEnd,
                                                      int yStart,
                                                      int yEnd,
                                                      const Vector2f& origin) {
  std::vector<Vector2f> result;
  int xLow = std::min(xStart, xEnd);
  int xHigh = std::max(xStart, xEnd);

  int yLow = std::min(yStart, yEnd);
  int yHigh = std::max(yStart, yEnd);

  float left = xLow * tileSize;
  float right = (xHigh + 1) * tileSize;
  float top = yLow * tileSize;
  float bottom = (yHigh + 1) * tileSize;
  Vector2f topLeft(left, top);
  Vector2f topRight(right, top);
  Vector2f botLeft(left, bottom);
  Vector2f botRight(right, bottom);

  Vector2i originTile = Tile(origin);
  if (originTile.x < xLow) {
    if (originTile.y < yLow) {
      result.push_back(topRight);      
      result.push_back(topLeft);
      result.push_back(botLeft);
    }
    else {
      result.push_back(topLeft);
      result.push_back(botLeft);
      if (originTile.y > yHigh) {
        result.push_back(botRight);
      }
    }
  } else if (originTile.x > xHigh) {
    if (originTile.y <= yHigh) {
      result.push_back(botRight);
      result.push_back(topRight);
      if (originTile.y < yLow) {
        result.push_back(topLeft);
      }
    } else {
      result.push_back(botLeft);
      result.push_back(botRight);
      result.push_back(topRight);
    }
  } else {
    if (originTile.y < yLow) {
      result.push_back(topRight);
      result.push_back(topLeft);
    } else {
      result.push_back(botLeft);
      result.push_back(botRight);
    }
  }

  return result;
}

Vector2i ShadowCaster::Tile(const Vector2f& coords) const {
  return Vector2i(int(coords.x / tileSize), int(coords.y / tileSize));
}

int ShadowCaster::FindFirstWall(int column,
                                int startRow,
                                int endRow,
                                bool flipY) {
  int direction = 1;
  if (flipY) direction = -1;

  for (int row = startRow; direction * (row - endRow) <= 0; row += direction) {
    if (!grid.IsUnblocked(column, row)) {
      return row;
    }
  }
  return endRow + direction;
}

int ShadowCaster::FindFirstOpen(int column,
                                int startRow,
                                int endRow,
                                bool flipY) {
  int direction = 1;
  if (flipY) direction = -1;

  for (int row = startRow; direction * (row - endRow) <= 0; row += direction) {
    if (grid.IsUnblocked(column, row)) {
      return row;
    }
  }
  return endRow + direction;
}

// TODO: Remove repetition.
int ShadowCaster::FindHighRow(const Vector2f& origin,
                              float endAngle,
                              float distance,
                              int column,
                              bool flipX) {

  // Column intersection requires that the angle not be vertical.
  if (endAngle == M_PI / 2)
    return grid.Size().y;

  int xDir = flipX ? -1 : 1;
  float h1 = FindColumnIntersection(origin, endAngle, column).y / tileSize;
  float h2 = FindColumnIntersection(origin, endAngle, column + 1).y / tileSize;
  int row = int(std::max(h1, h2));
  if (row >= grid.Size().y) row = grid.Size().y;
  return row;
}

int ShadowCaster::FindLowRow(const Vector2f& origin,
                              float startAngle,
                              float distance,
                              int column,
                              bool flipX) {

  // Column intersection requires that the angle not be vertical.
  if (startAngle == 3 * M_PI / 2)
    return -1;
  
  int xDir = flipX ? -1 : 1;
  float h1 = FindColumnIntersection(origin, startAngle, column).y / tileSize;
  float h2 = FindColumnIntersection(origin, startAngle, column + 1).y / tileSize;
  int row = int(std::floor(std::min(h1, h2)));
  if (row < 0) row = -1;
  return row;
}