#pragma once
#include <vector>
#include <deque>
#include "Vector2.h"

class PathingGrid;

class ShadowCaster {
  public:
    ShadowCaster(const PathingGrid& grid, float tileSize) :
        grid(grid), tileSize(tileSize) { }

    // Returns a list of pairs of points representing visible triangles.
    std::vector<Vector2f> ShadowCast(const Vector2f& origin, float startAngle,
        float endAngle, float distance);

  private:
    Vector2i Tile(const Vector2f& coords) const;

    int FindHighRow(const Vector2f& origin, float endAngle, float distance,
        int column, bool flipY);

    int FindLowRow(const Vector2f& origin, float startAngle, float distance,
        int column, bool flipX);

    // Returns, in CCW order the corner points of the specified grid rectangle.
    std::vector<Vector2f> GetVisibleCorners(int xStart, int xEnd, int yStart,
        int yEnd, const Vector2f& origin);

    // Returns the row of the first wall in the column between the start and
    // end rows (inclusive). Returns endRow + 1 if no walls are found.
    int FindFirstWall(int column, int startRow, int endRow, bool flipY);

    // Returns the first unblocked row in the column between the start and end
    // rows (inclusive). Returns endRow + 1 if no open tiles exist.
    int FindFirstOpen(int column, int startRow, int endRow, bool flipY);

    void ShadowCast(Vector2f origin, float startAngle,
        float endAngle, float distance, int column, bool flipX, bool flipY);

    // This quickly finds the intersection of the line from the origin through
    // the given angle with the line containing p1 and p2. p1 and p2 must be
    // on a row or grid line.
    Vector2f FindIntersection(const Vector2f& origin, float angle,
        const Vector2f& p1, const Vector2f& p2) const;

    Vector2f FindRowIntersection(const Vector2f& origin, float angle,
        int row) const;

    Vector2f FindColumnIntersection(const Vector2f& origin, float angle,
        int column) const;


    const PathingGrid& grid;
    float tileSize;

    struct ColumnSector {
      int column;
      float startAngle, endAngle;
      bool flipX, flipY;
    };

    void QueueColumnSector(int column, float startAngle, float endAngle,
        bool flipX, bool flipY);

    std::deque<ColumnSector> sectorsToCheck;

    // This is used to store the visibility polygon. The visibility polygon
    // can be drawn by drawing each triangle defined by the origin of the test,
    // visiblePoints[n], and visiblePoints[n+1] where n is even.
    std::vector<Vector2f> visiblePoints;

    // This stores the results of the line segment visibility test. Each pair
    // of floats represents the start and end angles of a visible sector of the
    // input segment.
    std::vector<float> visibleSectors;

    // Takes the start and end angles of two sectors, in CCW order, and returns
    // the start and end angles of the intersection sector, or an empty vector
    // if there is no intersection.
    static std::vector<float> FindSectorIntersection(float s1, float e1,
        float s2, float e2);
};