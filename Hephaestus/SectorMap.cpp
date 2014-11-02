#include "stdafx.h"
#include "SectorMap.h"
#include "Util.h"
#include "Circle.h"
#include "Timer.h"

typedef SectorMap::Sector Sector;
typedef std::vector<Sector>::const_reverse_iterator iterator;

Sector Nil = Sector();

bool Sector::IsNil() const {
  return startAngle == 0 && endAngle == 0 && startDepth == 0 && endDepth == 0;
}

SectorMap::SectorMap() { }

float SectorDepthAtAngle(const SectorMap::Sector &sector, float angle) {
  if (sector.endAngle < sector.startAngle) {
    float sizeCcwFrom0 = 2 * M_PI - sector.startAngle;
    SectorMap::Sector newSector;
    newSector.startAngle = 0;
    newSector.endAngle = sector.endAngle + sizeCcwFrom0;
    newSector.startDepth = sector.startDepth;
    newSector.endDepth = sector.endDepth;
    float depth = SectorDepthAtAngle(newSector, angle + sizeCcwFrom0);
    assert(depth > 0);
    return depth;
  }

  assert(angle >= sector.startAngle);
  assert(angle <= sector.endAngle);

  float size = sector.endAngle - sector.startAngle;
  float sizeCwFromAngle = sector.endAngle - angle;
  float fractionCwFromAngle = sizeCwFromAngle / size;
  float depth = fractionCwFromAngle * sector.startDepth + (1 - fractionCwFromAngle) * sector.endDepth;
  assert(depth > 0);
  return depth;
}

void TrimSectorStart(Sector &sector, float startAngle) {
  assert(startAngle >= sector.startAngle);
  assert(startAngle <= sector.endAngle);
  sector.startDepth = SectorDepthAtAngle(sector, startAngle);
  sector.startAngle = startAngle;
}

void TrimSectorEnd(Sector &sector, float endAngle) {
  assert(endAngle >= sector.startAngle);
  assert(endAngle <= sector.endAngle);
  sector.endDepth = SectorDepthAtAngle(sector, endAngle);
  sector.endAngle = endAngle;
}

Sector Subsector(const Sector &sector, float startAngle, float endAngle) {
  assert(startAngle <= endAngle);

  Sector subsector = sector;
  TrimSectorStart(subsector, startAngle);
  TrimSectorEnd(subsector, endAngle);
  return subsector;
}

// Returns if sector1 is shallower than sector2.
bool IsSectorShallower(const Sector &sector1, const Sector &sector2) {
  bool hasSameStartPoint = sector1.startAngle == sector2.startAngle && sector1.startDepth == sector2.startDepth; 
  float comparisonAngle = hasSameStartPoint ? std::min(sector1.endAngle, sector2.endAngle) :
    std::max(sector1.startAngle, sector2.startAngle);

  return SectorDepthAtAngle(sector1, comparisonAngle) <= SectorDepthAtAngle(sector2, comparisonAngle);
}

void SectorMap::CreateInputSectors(const std::vector<LineSegment> &segments) {
  inputSectors.clear();

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
      inputSectors.push_back(sector);
    } else {
      // In this case the sector crosses the angle 0. This splits it into
      // the sectors CW and CCW from 0.
      Sector sector1;
      sector1.startAngle = sector.startAngle;
      sector1.endAngle = 2 * M_PI;
      sector1.startDepth = sector.startDepth;
      sector1.endDepth = SectorDepthAtAngle(sector, 0);
      inputSectors.push_back(sector1);

      Sector sector2;
      sector2.startAngle = 0;
      sector2.endAngle = sector.endAngle;
      sector2.startDepth = sector1.endDepth;
      sector2.endDepth = sector.endDepth;
      inputSectors.push_back(sector2);
    }
  }

}

struct {
  bool operator()(Sector a, Sector b) {
    return a.startAngle > b.startAngle;
  }
} startAngleGreater;

const Sector &SectorMap::Next(iterator &inputSector, iterator &sectorRemainder) const {
  if (sectorRemainder == sectorRemainders.crend()) {
    if (inputSector == inputSectors.crend()) {
      return Nil;
    }

    return *inputSector++;
  }   

  if (inputSector == inputSectors.crend())
    return *sectorRemainder++;

  if (inputSector->startAngle < sectorRemainder->startAngle) {
    return *inputSector++;
  } else {
    return *sectorRemainder++;
  }
}

// Returns first sector which occludes any part of the input sector.
const Sector &SectorMap::FirstOccludingSector(const Sector &sector) const {
  auto sector1 = inputSectors.crbegin();
  auto sector2 = sectorRemainders.crbegin();

  for (const Sector *nextSector = &Next(sector1, sector2); !nextSector->IsNil()
    && nextSector->startAngle < sector.endAngle; nextSector = &Next(sector1, sector2)) {

    if (nextSector->endAngle > sector.startAngle && IsSectorShallower(*nextSector, sector))
      return *nextSector;
  }

  return Nil;
}

Sector PopBack(std::vector<Sector> &vector) {
  auto sector = vector.back();
  vector.pop_back();
  return sector;
}

// Returns the first sector which ends after the end of the input sector.
Sector SectorMap::PopNextSector() {
  if (inputSectors.empty()) {
    if (sectorRemainders.empty()) {
      return Nil;
    } else {
      return PopBack(sectorRemainders);
    }
  }

  if (sectorRemainders.empty())
    return PopBack(inputSectors);

  if (inputSectors.back().startAngle < sectorRemainders.back().startAngle) {
    return PopBack(inputSectors);
  } else {
    return PopBack(sectorRemainders);
  }
}

// Assumes that there is no ray from the center which does not intersect a segment.
// Assumes no segments intersect.
void SectorMap::Create(const Vector2f &center, const std::vector<LineSegment> &segments) {  
  this->center = center;
  sectors.clear();
  sectorRemainders.clear();

  CreateInputSectors(segments);

  // Sort in reverse order so we can pop elements off the back;
  std::sort(inputSectors.begin(), inputSectors.end(), startAngleGreater);

  auto sector1 = PopNextSector();
  while (true) {
    auto sector2 = FirstOccludingSector(sector1);
    if (!sector2.IsNil()) {
      if (sector2.startAngle < sector1.startAngle)
        TrimSectorStart(sector2, sector1.startAngle);

      if (sector1.endAngle > sector2.endAngle) {
        Sector sector1Remainder = Subsector(sector1, sector2.endAngle, sector1.endAngle);
        auto insertionLocation = std::lower_bound(sectorRemainders.begin(), sectorRemainders.end(), sector1Remainder, startAngleGreater);
        sectorRemainders.insert(insertionLocation, sector1Remainder);
      }
      TrimSectorEnd(sector1, sector2.startAngle);
    }
    if (sector1.endAngle > sector1.startAngle)
      sectors.push_back(sector1);

    // Get the next sector which is not fully occluded by sector1.
     do {    
      sector2 = PopNextSector();
      if (sector2.IsNil())
        return;
    } while (sector2.endAngle <= sector1.endAngle);

    TrimSectorStart(sector2, sector1.endAngle);
    sector1 = sector2;
  }
}

VisibilityPolygon SectorMap::AsPolygon() const {
  std::vector<Vector2f> vertices(2 * sectors.size());
  for (auto sector : sectors) {
    vertices.push_back(sector.startDepth * Util::MakeUnitVector(sector.startAngle) + center);
    vertices.push_back(sector.endDepth * Util::MakeUnitVector(sector.endAngle) + center);
  }

  return VisibilityPolygon(center, vertices);
}