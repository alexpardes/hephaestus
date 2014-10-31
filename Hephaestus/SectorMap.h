#pragma once
#include "Vector2.h"
#include <map>
#include "Util.h"

// This class creates the visibility polygon for a point from a set of line segments.
class SectorMap {
  public:
    struct Sector;
    SectorMap();

    // Creates the visibility polygon.
    void Create(const Vector2f& center, const std::vector<LineSegment> &segments);
    bool Contains(const Vector2f &point) const;
    Vector2f Center() const { return center; }

  private:
    void CreateSectors(const std::vector<LineSegment> &segments);
    std::vector<SectorMap::Sector>::iterator NextVisibleSector(const std::vector<SectorMap::Sector>::iterator &sector);

    Vector2f center;
    std::vector<Sector> rawSectors;
    std::vector<Sector> sectors;
};

// Compare performance of implementations.
struct SectorMap::Sector {  
  float startAngle, endAngle, startDepth, endDepth;
  //Vector2f startPoint, endPoint;
};