#include "stdafx.h"
#include "SectorMap.h"
#include "Util.h"
#include "Circle.h"

SectorMap::SectorMap() {
  Clear();
}

SectorMap::SectorMap(const Vector2f& center) : center(center) {
  Clear();
}

SectorMap::SectorMap(const SectorMap& map) {
  center = map.center;
  tree = map.tree;
}

bool SectorMap::Contains(const Vector2f& point) const {
  float angle = Util::FindAngle(point - center);
  float depth = Util::Distance(point, center);
  return GetSectorDepth(angle) >= depth;
}

bool SectorMap::IntersectsCircle(const Vector2f& center,
                                 float radius) const {
  std::pair<float, float> intersection = LargestCircleIntersection(center,
      radius);
  return !(intersection.first == intersection.second);

  //Circle circle(center, radius);
  //std::vector<Vector2f> widestPoints = circle.WidestPoints(this->center);
  //float startAngle = Util::FindAngle(widestPoints[0] - this->center);
  //float endAngle = Util::FindAngle(widestPoints[1] - this->center);
  //float distance = Util::Distance(this->center, center) - radius;

  //bool intersectsCircle = false;
  //std::shared_ptr<const Sector> sector = GetSector(startAngle);
  //do {
  //  if (sector->Depth() > distance) {
  //    intersectsCircle = true;
  //    break;
  //  }
  //  sector = sector->Next();
  //} while (!sector->ContainsAngle(endAngle));

  //return intersectsCircle;
}

std::pair<float, float> SectorMap::LargestCircleIntersection(
    const Vector2f& center, float radius) const {

  Circle circle(center, radius);
  std::vector<Vector2f> widestPoints = circle.WidestPoints(this->center);
  float startAngle = Util::FindAngle(widestPoints[0] - this->center);
  float endAngle = Util::FindAngle(widestPoints[1] - this->center);
  float distance = Util::Distance(this->center, center) - radius;

  std::pair<float, float> result(0.f, 0.f);

  float maxSectionSize = 0;
  Sector sector = GetSector(startAngle);
  float currentStartAngle = startAngle;
  bool combineWithPrevious = false;

  while (!sector.ContainsAngle(endAngle)) {
    if (sector.Depth() < distance) {
      if (combineWithPrevious) {
        float currentEndAngle = sector.StartAngle();

        float intersectionSize = Util::AngleCCW(currentStartAngle,
            currentEndAngle);
        if (intersectionSize > maxSectionSize) {
          result.first = currentStartAngle;
          result.second = currentEndAngle;
        }
      }

      currentStartAngle = sector.EndAngle();
      combineWithPrevious = false;
    } else {
      combineWithPrevious = true;
    }
    sector = sector.Next();
  }

  float currentEndAngle;
  if (sector.Depth() < distance) {
    currentEndAngle = sector.StartAngle();
  } else {
    currentEndAngle = endAngle;
  }

  float intersectionSize = Util::AngleCCW(currentStartAngle,
    currentEndAngle);
  if (intersectionSize > maxSectionSize) {
    result.first = currentStartAngle;
    result.second = currentEndAngle;
  }


  return result;
}

void SectorMap::Clear() {
  tree.clear();
  Sector sector(&tree);
}

void SectorMap::Add(Sector startSector,
                    float startAngle,
                    float endAngle,
                    float depth) {
  Sector nextSector;
  bool containsNewSector = Util::IsBetweenAngles(endAngle,
    startSector.StartAngle(), startSector.EndAngle());

  float originalDepth = startSector.Depth();
  if (depth < originalDepth) {
    Sector newSector;
    if (startAngle != startSector.StartAngle()) {
      startSector.PostInsert(startAngle, depth);
      newSector = startSector.Next();
    } else {
      newSector = startSector;
      newSector.SetDepth(depth);
    }

    // Creates a new sector representing the remainder of the start sector.
    if (containsNewSector) {
      newSector.PostInsert(endAngle, originalDepth);
    } else {
      nextSector = newSector.Next();
    }
  } else {
    nextSector = startSector.Next();
  }

  if (!containsNewSector) {
    if (endAngle != nextSector.StartAngle()) {
      Add(nextSector, nextSector.StartAngle(), endAngle, depth);
    }
  }
}


// TODO: make sure there are no remaining edge cases to handle.
// TODO: combine adjacent sectors of equal depth or otherwise minimize number
// of sectors.
void SectorMap::Add(float startAngle, float endAngle, float depth) {
  Sector startSector = GetSector(startAngle);
  Add(startSector, startAngle, endAngle, depth);
}

SectorMap::Sector SectorMap::GetSector(float angle) {  
  std::map<float, float>::iterator it = tree.upper_bound(angle);
  if (it == tree.end()) {
    it = tree.begin();
  }
  Sector sector = Sector(&tree, it).Prev();

  return sector;
}

const SectorMap::Sector SectorMap::GetSector(float angle) const {
  // Because the non-const version should not modify anything, this should be
  // safe.
  return const_cast<SectorMap*>(this)->GetSector(angle);
}

float SectorMap::GetSectorDepth(float angle) const {
  return GetSector(angle).Depth();
}