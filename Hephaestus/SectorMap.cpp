#include "stdafx.h"
#include "SectorMap.h"
#include "Util.h"

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

void SectorMap::Clear() {
  tree.clear();
  Sector sector(&tree);
}


// TODO: make sure there are no remaining edge cases to handle.
// TODO: combine adjacent sectors of equal depth or otherwise minimize number
// of sectors.
void SectorMap::Add(float startAngle, float endAngle, float depth) {
  Sector* nextSector = nullptr;
  Sector* startSector = GetSector(startAngle);
  bool containsNewSector = Util::IsBetweenAngles(endAngle,
      startSector->StartAngle(), startSector->EndAngle());

  float originalDepth = startSector->Depth();
  if (depth < originalDepth) {
    Sector* newSector = nullptr;
    if (startAngle != startSector->StartAngle()) {
      startSector->PostInsert(startAngle, depth);
      newSector = startSector->Next();
    } else {
      newSector = startSector;
      newSector->SetDepth(depth);
    }

    // Creates a new sector representing the remainder of the start sector.
    if (containsNewSector) {
      newSector->PostInsert(endAngle, originalDepth);
    } else {
      nextSector = newSector->Next();
    }
  } else {
    nextSector = startSector->Next();
  }

  if (!containsNewSector) {
    if (endAngle != nextSector->StartAngle()) {
      Add(nextSector->StartAngle(), endAngle, depth);
    }
  }
}

SectorMap::Sector* SectorMap::GetSector(float angle) {  
  std::map<float, float>::iterator it = tree.upper_bound(angle);
  if (it == tree.end()) {
    it = tree.begin();
  }
  Sector* sector = Sector(&tree, it).Prev();

  return sector;
}

const SectorMap::Sector* SectorMap::GetSector(float angle) const {
  // Because the non-const version should not modify anything, this should be
  // safe.
  return const_cast<SectorMap*>(this)->GetSector(angle);
}

float SectorMap::GetSectorDepth(float angle) const {
  return GetSector(angle)->Depth();
}