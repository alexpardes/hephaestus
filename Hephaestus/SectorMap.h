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
    VisibilityPolygon ToPolygon() const;
    bool IsAnyVisible (const LineSegment &segment) const;
    LineSegment LargestVisibleSubsegment(const LineSegment &segment) const;

    // Returns the sum of the angular sizes of all visible subsegments of the input segment.
    float VisibleSize(const LineSegment &segment) const;

  private:
    typedef std::vector<Sector>::const_reverse_iterator iterator;
    std::vector<Sector> VisibleSubsectors(const Sector &sector) const;
    std::vector<Sector> VisibleSubsectors(const LineSegment &segment) const;
    const Sector &Next(iterator &sector1, iterator &sector2) const;
    void CreateInputSectors(const std::vector<LineSegment> &segments);
    Sector PopNextSector();
    const Sector &FirstOccludingSector(const Sector &sector) const;

    Vector2f center;
    // These two arrays do not represent any particular information. They are class members only to reduce dynamic allocation.
    std::vector<Sector> tempSectors1, tempSectors2;
    std::vector<Sector> sectors; // This represents the finished visibility polygon.
};

struct SectorMap::Sector {
  Sector () { }
  Sector(float startAngle, float endAngle, float startDepth, float endDepth) :
    startAngle(startAngle), endAngle(endAngle), startDepth(startDepth), endDepth(endDepth) { }  

  bool IsNil() const;
  float Size() const;

  float startAngle, endAngle, startDepth, endDepth;
};