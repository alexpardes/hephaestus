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

float Sector::Size() const {
  if (endAngle >= startAngle)
    return endAngle - startAngle;

  return 2 * M_PI - startAngle + endAngle;
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
    float depth = SectorDepthAtAngle(newSector, Util::Angle(angle + sizeCcwFrom0));
    assert(depth > 0);
    return depth;
  }

  assert(angle >= sector.startAngle);
  assert(angle <= sector.endAngle);

  if (sector.startAngle == sector.endAngle)
    return sector.startDepth;

  float size = sector.endAngle - sector.startAngle;
  float sizeCwFromAngle = sector.endAngle - angle;
  float fractionCwFromAngle = sizeCwFromAngle / size;
  float depth = fractionCwFromAngle * sector.startDepth + (1 - fractionCwFromAngle) * sector.endDepth;
  assert(depth > 0);
  return depth;
}

std::pair<Sector, Sector> SplitSectorAtZero(const Sector &sector) {
  Sector sector1, sector2;
  sector1.startAngle = sector.startAngle;
  sector1.startDepth = sector.startDepth;
  sector1.endAngle = 2 * M_PI;
  sector1.endDepth = SectorDepthAtAngle(sector, 0);

  sector2.startAngle = 0;
  sector2.startDepth = sector1.endDepth;
  sector2.endAngle = sector.endAngle;
  sector2.endDepth = sector.endDepth;
  return std::make_pair(sector1, sector2);
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

Sector SectorFromSegment(const LineSegment &segment, const Vector2f &center) {
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
  if (sector.endAngle == 0 && sector.startAngle != 0)
    sector.endAngle = 2 * M_PI;

  return sector;
}

void SectorMap::CreateInputSectors(const std::vector<LineSegment> &segments) {
  tempSectors1.clear();

  for (auto segment : segments) {
    auto sector = SectorFromSegment(segment, center);
    if (sector.startAngle <= sector.endAngle) {
      tempSectors1.push_back(sector);
    } else {
      auto splitSectors = SplitSectorAtZero(sector);
      tempSectors1.push_back(splitSectors.first);
      tempSectors1.push_back(splitSectors.second);
    }
  }
}

struct {
  bool operator()(Sector a, Sector b) {
    return a.startAngle > b.startAngle;
  }
} startAngleGreater;

const Sector &SectorMap::Next(iterator &inputSector, iterator &sectorRemainder) const {
  if (sectorRemainder == tempSectors2.crend()) {
    if (inputSector == tempSectors1.crend()) {
      return Nil;
    }

    return *inputSector++;
  }   

  if (inputSector == tempSectors1.crend())
    return *sectorRemainder++;

  if (inputSector->startAngle < sectorRemainder->startAngle) {
    return *inputSector++;
  } else {
    return *sectorRemainder++;
  }
}

// Returns first sector which occludes any part of the input sector.
const Sector &SectorMap::FirstOccludingSector(const Sector &sector) const {
  auto sector1 = tempSectors1.crbegin();
  auto sector2 = tempSectors2.crbegin();

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
  if (tempSectors1.empty()) {
    if (tempSectors2.empty()) {
      return Nil;
    } else {
      return PopBack(tempSectors2);
    }
  }

  if (tempSectors2.empty())
    return PopBack(tempSectors1);

  if (tempSectors1.back().startAngle < tempSectors2.back().startAngle) {
    return PopBack(tempSectors1);
  } else {
    return PopBack(tempSectors2);
  }
}

// Assumes that there is no ray from the center which does not intersect a segment.
// Assumes no segments intersect.
void SectorMap::Create(const Vector2f &center, const std::vector<LineSegment> &segments) {  
  this->center = center;
  sectors.clear();
  tempSectors2.clear();

  CreateInputSectors(segments);

  // Sort in reverse order so we can pop elements off the back;
  std::sort(tempSectors1.begin(), tempSectors1.end(), startAngleGreater);

  auto sector1 = PopNextSector();
  while (true) {
    auto sector2 = FirstOccludingSector(sector1);
    if (!sector2.IsNil()) {
      if (sector2.startAngle < sector1.startAngle)
        TrimSectorStart(sector2, sector1.startAngle);

      if (sector1.endAngle > sector2.endAngle) {
        Sector sector1Remainder = Subsector(sector1, sector2.endAngle, sector1.endAngle);
        auto insertionLocation = std::lower_bound(tempSectors2.begin(), tempSectors2.end(), sector1Remainder, startAngleGreater);
        tempSectors2.insert(insertionLocation, sector1Remainder);
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

VisibilityPolygon SectorMap::ToPolygon() const {
  std::vector<Vector2f> vertices(2 * sectors.size());
  for (auto sector : sectors) {
    vertices.push_back(sector.startDepth * Util::MakeUnitVector(sector.startAngle) + center);
    vertices.push_back(sector.endDepth * Util::MakeUnitVector(sector.endAngle) + center);
  }

  return VisibilityPolygon(center, vertices);
}

// Returns the angle of intersection of the line segments represented by the sectors.
float IntersectionAngle(const Sector &sector1, const Sector &sector2) {
  float s1 = sector1.startAngle;
  float s2 = sector2.startAngle;
  float e1 = sector1.endAngle;
  float e2 = sector2.endAngle;
  float vs1 = sector1.startDepth;
  float vs2 = sector2.startDepth;
  float ve1 = sector1.endDepth;
  float ve2 = sector2.endDepth;
  float l1 = e1 - s1;
  float l2 = e2 - s2;
  float n = (e1*ve1 - s1*vs1)/l1 - (e2*ve2 - s2*vs2)/l2;
  float d = (vs1 - ve1)/l1 - (vs2 - ve2)/l2;
  return n / d;
}

// Returns the portion of sector2 which is contained in sector1.
Sector SectorIntersection(const Sector &sector1, const Sector &sector2) {
  float startAngle = std::max(sector1.startAngle, sector2.startAngle);
  float endAngle = std::min(sector1.endAngle, sector2.endAngle);

  Sector sector2Trimmed = sector2;
  TrimSectorStart(sector2Trimmed, startAngle);
  TrimSectorEnd(sector2Trimmed, endAngle);

  float startDepth1 = SectorDepthAtAngle(sector1, startAngle);
  float endDepth1 = SectorDepthAtAngle(sector1, endAngle);

  bool startContained = sector2Trimmed.startDepth <= startDepth1;
  bool endContained = sector2Trimmed.endDepth <= endDepth1;

  if (!startContained && !endContained)
    return Nil;

  float angle = IntersectionAngle(sector1, sector2);
  if (!endContained) {
    TrimSectorEnd(sector2Trimmed, angle);
  } else if (!startContained) {
    TrimSectorStart(sector2Trimmed, angle);
  }
  return sector2Trimmed;
}

// If sector2 is immediately clockwise of sector1, modifies sector1 to include sector2 and returns true.
bool TryCombineSectors(Sector &sector1, const Sector &sector2) {
  if (sector1.endAngle != sector2.startAngle && (sector1.endAngle != 2*M_PI || sector2.startAngle != 0))
    return false;

  sector1.endAngle = sector2.endAngle;
  sector1.endDepth = sector2.endDepth;
  return true;
}

LineSegment SegmentFromSector(const Sector &sector, const Vector2f& center) {
  Vector2f start(sector.startDepth * Util::MakeUnitVector(sector.startAngle) + center);
  Vector2f end(sector.endDepth * Util::MakeUnitVector(sector.endAngle) + center);
  return LineSegment(start, end);
}

struct {
  bool operator()(Sector a, Sector b) {
    return a.startAngle < b.startAngle;
  }
} startAngleLess;

// Caller must not modify tempSectors1 while holding the return value of this method.
std::vector<Sector> SectorMap::VisibleSubsectors(const Sector &sector) const {
  std::vector<Sector> subsectors; // TODO: Remove this allocation.

  // This is the sector containing the start of the input sector.
  auto sector2 = std::upper_bound(sectors.begin(), sectors.end(), sector, startAngleLess) - 1;
  Sector lastSubsector = Nil;
  while (sector2 != sectors.end() && sector2->startAngle < sector.endAngle) {
    auto subsector = SectorIntersection(*sector2, sector);
    if (!subsector.IsNil() && !TryCombineSectors(lastSubsector, subsector)) {
      subsectors.push_back(lastSubsector);
      lastSubsector = subsector;
    }    
    ++sector2;
  }
  if (!lastSubsector.IsNil())
    subsectors.push_back(lastSubsector);

  return subsectors;
}

// TODO: Merge sectors at zero.
std::vector<Sector> SectorMap::VisibleSubsectors(const LineSegment &segment) const {
  std::vector<Sector> subsectors;
  auto segmentSector = SectorFromSegment(segment, center);
  if (segmentSector.endAngle < segmentSector.startAngle) {
    auto splitSectors = SplitSectorAtZero(segmentSector);
    subsectors = VisibleSubsectors(splitSectors.first);
    auto subsectors2 = VisibleSubsectors(splitSectors.second);

    // Since we artificially split at zero, attempts to reattach the sectors there.
    if (!subsectors.empty() && !subsectors2.empty() && TryCombineSectors(subsectors2.front(), subsectors.back()))
      subsectors.pop_back();

    subsectors.insert(subsectors.end(), subsectors2.begin(), subsectors2.end());
  } else {
    subsectors = VisibleSubsectors(segmentSector);
  }
  return subsectors;
}

bool SectorMap::IsAnyVisible(const LineSegment &segment) const {
  return !VisibleSubsectors(segment).empty();
}

LineSegment SectorMap::LargestVisibleSubsegment(const LineSegment &segment) const {
  Sector largestSector = Nil;
  for (auto sector : VisibleSubsectors(segment)) {
    if (sector.Size() > largestSector.Size())
      largestSector = sector;
  }
  return SegmentFromSector(largestSector, center);
}

float SectorMap::VisibleSize(const LineSegment &segment) const {
  float size = 0;
  for (auto sector : VisibleSubsectors(segment))
    size += sector.Size();

  return size;
}