#pragma once
#include "Vector2.h"
#include "Util.h"
#include "VisibilityPolygon.h"

// This class creates the visibility polygon for a point from a set of line segments.
class SectorMap {
  public:
    struct Sector;

    SectorMap();

    // Creates the visibility polygon.
    void Create(const Vector2f& center, const std::vector<LineSegment> &segments);
    bool Contains(const Vector2f &point) const;
    Vector2f Center() const { return center; }
    VisibilityPolygon AsPolygon() const;

  private:
    typedef std::vector<Sector>::const_reverse_iterator iterator;
    const Sector &Next(iterator &sector1, iterator &sector2) const;
    void CreateInputSectors(const std::vector<LineSegment> &segments);
    Sector PopNextSector();
    const Sector &FirstOccludingSector(const Sector &sector) const;

    Vector2f center;
    // These two arrays are only used while creating the visibility polygon. They are class members only to reduce dynamic allocation.
    std::vector<Sector> inputSectors, sectorRemainders;
    std::vector<Sector> sectors; // This represents the finished visibility polygon.
};

struct SectorMap::Sector {
  Sector () { }
  Sector(float startAngle, float endAngle, float startDepth, float endDepth) :
    startAngle(startAngle), endAngle(endAngle), startDepth(startDepth), endDepth(endDepth) { }  

  bool IsNil() const;

  float startAngle, endAngle, startDepth, endDepth;
};