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

Vector2f Point(float angle, float depth) {
  return depth * Util::MakeUnitVector(angle);
}

float Angle(const Vector2f &p, const Vector2f &center) {
  return Util::FindAngle(p - center);
}

LineSegment Segment(const SectorMap::Sector &sector) {
  return LineSegment(Point(sector.startAngle, sector.startDepth), Point(sector.endAngle, sector.endDepth));
}

float DepthAtAngle(const SectorMap::Sector &sector, float angle) {
  Vector2f p1 = Point(sector.startAngle, sector.startDepth);
  Vector2f p2 = Point(sector.endAngle, sector.endDepth);
  float t = (std::tan(p2.x) - p2.y) / (std::tan(p2.x - p1.x) + p1.y - p2.y);
  float term1 = std::pow(t*p1.x + (1 - t)*p2.x, 2);
  float term2 = std::pow(t*p1.y + (1 - t)*p2.y, 2);
  float depth = std::sqrt(term1 + term2);
  return depth;
}

float SectorDepthAtAngle(const SectorMap::Sector &sector, float angle) {
  assert(Util::IsBetweenAnglesClosed(angle, sector.startAngle, sector.endAngle));
  if (angle == sector.startAngle)
    return sector.startDepth;

  if (angle == sector.endAngle)
    return sector.endDepth;

  if (sector.startDepth == 0 && sector.endDepth == 0)
    return 0;

  auto v1 = Point(sector.startAngle, sector.startDepth);
  auto v2 = Point(sector.endAngle, sector.endDepth);
  auto direction = Util::MakeUnitVector(angle);
  auto intersection = LineSegment(v1, v2).GetLine().Intersection(Line(Vector2f(0, 0), direction));
  return Util::Length(intersection);
}

std::pair<Sector, Sector> SplitSector(const Sector &sector, float angle = 0) {
  Sector sector1, sector2;
  sector1.startAngle = sector.startAngle;
  sector1.startDepth = sector.startDepth;
  sector1.endAngle = angle == 0 ? 2 * M_PI : angle;
  sector1.endDepth = SectorDepthAtAngle(sector, angle);

  sector2.startAngle = angle;
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
  //assert(endAngle >= sector.startAngle);
  //assert(endAngle <= sector.endAngle);
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

Sector SectorMap::SectorFromSegment(const LineSegment &segment) const {
  Vector2f v1 = segment.p1 - center;
  Vector2f v2 = segment.p2 - center;
  if (v1 == Vector2f(0, 0) || v2 == Vector2f(0, 0))
    return Nil;

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

  AssertSectorValid(sector);
  return sector;
}

void SectorMap::AddSector(const Sector &sector) {
  if (sector.Size() > M_PI / 2) {
    float midAngle = Util::InterpolateAnglesCcw(sector.startAngle, sector.endAngle, 0.5);
    auto splitSectors = SplitSector(sector, midAngle);
    AddSector(splitSectors.first);
    AddSector(splitSectors.second);
  } else if (sector.startAngle > sector.endAngle) {
    auto splitSectors = SplitSector(sector);
    AddSector(splitSectors.first);
    AddSector(splitSectors.second);
  }
  else {
    AssertSectorValid(sector);
    if (!sector.IsNil())
      tempSectors1.push_back(sector);
  }
}

/* Gives the sector blocked by a polygon vertex when the sectormap is placed at
 * the vertex. */
Sector SectorFromVertex(const Poly::Vertex &vertex) {
  float startAngle = Angle(vertex.Next().Point(), vertex.Point());
  float endAngle = Angle(vertex.Prev().Point(), vertex.Point());
  auto sector = Sector(startAngle, endAngle, 0, 0);
  return sector;
}

void SectorMap::CreateInputSectors(const std::vector<const Poly> &polygons) {
  tempSectors1.clear();

  for (auto polygon : polygons)
    for (auto vertex : polygon) {
      Sector sector;
      if (vertex.Point() == center) {
        sector = SectorFromVertex(vertex);
      } else if (vertex.Next().Point() == center) {
        sector = SectorFromVertex(vertex.Next());
      } else {
        auto segment = vertex.Segment();

        if (segment.Distance(center) < 0.001) {
          sector.startAngle = Angle(vertex.Next().Point(), center);
          sector.endAngle = Angle(vertex.Point(), center);
          sector.startDepth = 0;
          sector.endDepth = 0;
          AssertSectorValid(sector);
        } else {
          sector = SectorFromSegment(segment);
          if (sector.IsNil())
            continue;
        }
      }
      AddSector(sector);
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
void SectorMap::Create(const Vector2f &center, const std::vector<const Poly> &polygons) {  
  this->center = center;
  sectors.clear();
  tempSectors2.clear();

  CreateInputSectors(polygons);

  // Sort in reverse order so we can pop elements off the back;
  std::sort(tempSectors1.begin(), tempSectors1.end(), startAngleGreater);

  auto sector1 = PopNextSector();
  while (true) {
    AssertSectorValid(sector1);
    auto sector2 = FirstOccludingSector(sector1);
    if (!sector2.IsNil()) {
      if (sector2.startAngle < sector1.startAngle)
        TrimSectorStart(sector2, sector1.startAngle);

      if (sector1.endAngle > sector2.endAngle) {
        Sector sector1Remainder = Subsector(sector1, sector2.endAngle, sector1.endAngle);
        AssertSectorValid(sector1Remainder);
        auto insertionLocation = std::lower_bound(tempSectors2.begin(), tempSectors2.end(), sector1Remainder, startAngleGreater);
        tempSectors2.insert(insertionLocation, sector1Remainder);
      }
      TrimSectorEnd(sector1, sector2.startAngle);
    }
    if (sector1.endAngle > sector1.startAngle) {
      AssertSectorValid(sector1);
      sectors.push_back(sector1);
    }

    // Get the next sector which is not fully occluded by sector1.
     do {    
      sector2 = PopNextSector();
      if (sector2.IsNil())
        return;
    } while (sector2.endAngle <= sector1.endAngle);

    TrimSectorStart(sector2, sector1.endAngle);
    sector1 = sector2;
    AssertSectorValid(sector1);
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
  //float s1 = sector1.startAngle;
  //float s2 = sector2.startAngle;
  //float e1 = sector1.endAngle;
  //float e2 = sector2.endAngle;
  //float vs1 = sector1.startDepth;
  //float vs2 = sector2.startDepth;
  //float ve1 = sector1.endDepth;
  //float ve2 = sector2.endDepth;
  //float l1 = e1 - s1;
  //float l2 = e2 - s2;
  //float n = (e1*ve1 - s1*vs1)/l1 - (e2*ve2 - s2*vs2)/l2;
  //float d = (vs1 - ve1)/l1 - (vs2 - ve2)/l2;
  //return n / d;

  float startAngle = std::max(sector1.startAngle, sector2.startAngle);
  float endAngle = std::min(sector1.endAngle, sector2.endAngle);
  Vector2f intersection = Segment(sector1).GetLine().Intersection(Segment(sector2).GetLine());
  float angle = Util::FindAngle(intersection);

  // Necessary to clamp due to slight errors in calculation.
  if (angle < startAngle) {
    assert(startAngle - angle < 0.05);
    angle = startAngle;
  } else if (angle > endAngle) {
    assert(angle - endAngle < 0.05);
    angle = endAngle;
  }
  return angle;
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

  /* For the purpose of pathfinding we want points on the boundary to be
   * visible, and an epsilon is needed to ensure that. */
  const float kEpsilon = 0.1f;
  bool startContained = sector2Trimmed.startDepth <= startDepth1 + kEpsilon;
  bool endContained = sector2Trimmed.endDepth <= endDepth1 + kEpsilon;

  if (startContained && endContained)
    return sector2Trimmed;

  if (!startContained && !endContained)
    return Nil;

  float angle = IntersectionAngle(sector1, sector2);
  if (!endContained) {
    TrimSectorEnd(sector2Trimmed, angle);
  } else {
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

LineSegment SectorMap::SegmentFromSector(const Sector &sector) const {
  return LineSegment(GetPoint(sector.startAngle, sector.startDepth), GetPoint(sector.endAngle, sector.endDepth));
}

struct {
  bool operator()(Sector a, Sector b) {
    return a.startAngle < b.startAngle;
  }
} startAngleLess;

Sector SectorMap::SectorFromPoint(const Vector2f &point) const {
  auto angle = Util::FindAngle(point - center);
  auto distance = Util::Distance(point, center);
  return Sector(angle, angle, distance, distance);
}

bool SectorMap::IsVisible(const Vector2f &point) const {
  auto sector = SectorFromPoint(point);
  if (VisibleSubsectors(sector).size()) {
    return true;
  }
  if (sector.startAngle != 0)
    return false;

  /* If the point has angle zero, then it is possible that it is not contained
   * in the first sector but is contained in the last one. */
  sector.startAngle = 2 * M_PI;
  sector.endAngle = sector.startAngle;
  return VisibleSubsectors(sector).size() != 0;
}

std::vector<Sector> SectorMap::VisibleSubsectors(const Sector &sector) const {
  std::vector<Sector> subsectors;
  auto sector2 = std::lower_bound(sectors.begin(), sectors.end(), sector, startAngleLess);
  if (sector2 != sectors.begin())
    --sector2;

  Sector lastSubsector = Nil;
  while (sector2 != sectors.end() && sector2->startAngle <= sector.endAngle) {
    auto subsector = SectorIntersection(*sector2, sector);
    if (lastSubsector.IsNil()) {
      lastSubsector = subsector;
    } else if (!subsector.IsNil() && !TryCombineSectors(lastSubsector, subsector)) { 
        subsectors.push_back(lastSubsector);
        lastSubsector = subsector;
    }
    ++sector2;
  }
  if (!lastSubsector.IsNil())
    subsectors.push_back(lastSubsector);

  return subsectors;
}

std::vector<Sector> SectorMap::VisibleSubsectors(const LineSegment &segment) const {
  std::vector<Sector> subsectors;
  auto segmentSector = SectorFromSegment(segment);
  if (segmentSector.endAngle < segmentSector.startAngle) {
    auto splitSectors = SplitSector(segmentSector);
    subsectors = VisibleSubsectors(splitSectors.first);
    auto subsectors2 = VisibleSubsectors(splitSectors.second);

    auto begin2 = subsectors2.begin();

    // Since we artificially split at zero, attempts to reattach the sectors there.
    if (!subsectors.empty() && !subsectors2.empty() && TryCombineSectors(subsectors.back(), subsectors2.front()))
      ++begin2;

    subsectors.insert(subsectors.end(), begin2, subsectors2.end());
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
  return SegmentFromSector(largestSector);
}

std::vector<const LineSegment> SectorMap::VisibleSubsegments(const LineSegment &segment) const {
  std::vector<const LineSegment> subsegments;
  for (auto sector : VisibleSubsectors(segment)) {
    subsegments.push_back(SegmentFromSector(sector));
  }
  return subsegments;
}

std::vector<const LineSegment> SectorMap::VisibleSubsegments(const std::vector<const LineSegment> &segments) const {
  std::vector<const LineSegment> allSubsegments;
  for (auto segment : segments) {
    auto subsegments = VisibleSubsegments(segment);
    allSubsegments.insert(allSubsegments.end(), subsegments.begin(), subsegments.end());
  }
  return allSubsegments;
}

float SectorMap::VisibleSize(const LineSegment &segment) const {
  float size = 0;
  for (auto sector : VisibleSubsectors(segment))
    size += sector.Size();

  return size;
}

Vector2f SectorMap::GetPoint(float angle, float depth) const {
  return depth * Util::MakeUnitVector(angle) + center;
}

std::vector<const LineSegment> SectorMap::PolygonBorder() const {
  std::vector<const LineSegment> border;
  for (size_t i = 0; i < sectors.size() - 1; ++i) {
    if (sectors[i].endDepth != sectors[i + 1].startDepth) {
      float angle = sectors[i].endAngle;
      border.push_back(LineSegment(GetPoint(angle, sectors[i].endDepth), GetPoint(angle, sectors[i + 1].startDepth)));
    }
  }
  return border;
}

void SectorMap::AssertSectorValid(const Sector &sector) const {
  //const float kMin = -0.5f, kMax = 2000.5f;
  //auto startPoint = GetPoint(sector.startAngle, sector.startDepth);
  //auto endPoint = GetPoint(sector.endAngle, sector.endDepth);
  //assert(startPoint.x > kMin);
  //assert(startPoint.x < kMax);
  //assert(startPoint.y > kMin);
  //assert(startPoint.y < kMax);
  //assert(endPoint.x > kMin);
  //assert(endPoint.x < kMax);
  //assert(endPoint.y > kMin);
  //assert(endPoint.y < kMax);
}