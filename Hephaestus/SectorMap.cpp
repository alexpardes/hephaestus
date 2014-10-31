#include "stdafx.h"
#include "SectorMap.h"
#include "Util.h"
#include "Circle.h"
#include "Timer.h"

SectorMap::SectorMap() { }

void SectorMap::CreateSectors(const std::vector<LineSegment> &segments) {
  rawSectors.clear();

  for (auto segment : segments) {
    Vector2f v1 = segment.p1 - center;
    Vector2f v2 = segment.p2 - center;
    float angle1 = Util::FindAngle(v1);
    float angle2 = Util::FindAngle(v2);
    Sector sector;

    // startAngle is the angle on the clockwise side of the segment.
    if (Util::AngleCCW(angle1, angle2) < M_PI)
    {
      sector.startAngle = angle1;
      sector.endAngle = angle2;
      sector.startDepth = Util::Length(v1);
      sector.endDepth = Util::Length(v2);
    } else {
      sector.startAngle = angle2;
      sector.endAngle = angle1;
      sector.startDepth = Util::Length(v2);
      sector.endDepth = Util::Length(v1);
    }

    if (sector.startAngle <= sector.endAngle) {
      rawSectors.push_back(sector);
    } else {
      // In this case the sector crosses the angle 0. This splits it into
      // the sectors before and after 0.
      Sector sector1;
      sector1.startAngle = sector.startAngle;
      sector1.endAngle = 2*M_PI;
      sector1.startDepth = sector.startDepth;
      //sector1.endDepth = depth at zero
      rawSectors.push_back(sector1);

      Sector sector2;
      sector2.startAngle = 0;
      sector2.endAngle = sector.endAngle;
      sector2.startDepth = sector1.endDepth;
      sector2.endDepth = sector.endDepth;
      rawSectors.push_back(sector2);
    }
  }

}

struct {
  bool operator()(SectorMap::Sector a, SectorMap::Sector b) {
    return a.startAngle < b.startAngle;
  }
} startAngleLess;

// Returns the next sector which is not fully occluded by the input sector.
std::vector<SectorMap::Sector>::iterator SectorMap::NextVisibleSector(const std::vector<SectorMap::Sector>::iterator &sector) {
  auto sector2 = sector + 1;
  while (sector2 != rawSectors.end() && sector2->startDepth > sector->startDepth
      && sector2->endAngle <= sector->endAngle)
    ++sector2;

  return sector2;
}

// Assumes that there is no ray from the center which does not intersect a segment.
// Currently only uses start depth.
// Currently does not handle segments which cross the angle 0.
void SectorMap::Create(const Vector2f &center, const std::vector<LineSegment> &segments) {  
  this->center = center;
  sectors.clear();

  CreateSectors(segments);
  std::sort(rawSectors.begin(), rawSectors.end(), startAngleLess);

  auto sector1 = rawSectors.begin();
  while (sector1 != rawSectors.end()) {
    auto sector2 = NextVisibleSector(sector1);

    if (sector2 == rawSectors.end()) {
      sectors.push_back(*sector1);
      break;
    }

    bool isSector1Closer = sector1->startDepth <= sector2->startDepth;

    Sector sector;
    sector.startAngle = sector1->startAngle;
    sector.startDepth = sector1->startDepth;

    if (isSector1Closer) {
      sector.endAngle = sector1->endAngle;
      sector.endDepth = sector1->endDepth;
    } else {
      sector.endAngle = sector2->startAngle;
      sector.endDepth = sector2->startDepth;

      if (sector1->endAngle > sector2->endAngle) {
        // In this case part of sector1 may be visible past sector2 and needs to be processed again.
        // This moves sector2 one slot left, and moves the remainder of sector1 to the original slot of sector2.
        sector1->startAngle = sector2->endAngle;
        if (sector1 == sector2 - 1) {
          std::iter_swap(sector1, sector2);
        } else {
          *(sector2 - 1) = *sector2;
          *sector2 = *sector1;
        }
        --sector2; // Have to update the iterator to point to the right place.
      }
    }

    sector1 = sector2;
    sectors.push_back(sector);
  }
}