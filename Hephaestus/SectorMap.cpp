#include "stdafx.h"
#include "SectorMap.h"
#include "Util.h"

SectorMap::SectorMap() {
  headSector = new Sector(0.f, FLT_MAX);
  Clear();
}

SectorMap::SectorMap(const Vector2f& center) : center(center) {
  headSector = new Sector(0.f, FLT_MAX);
  Clear();
}

SectorMap::SectorMap(const SectorMap& map) {
  center = map.center;

  Sector* startSector = map.headSector;
  headSector = new Sector(*startSector);

  Sector* otherSector = startSector->next;
  Sector* copiedSector = headSector;
  while (otherSector != startSector) {
    copiedSector->PostInsert(new Sector(*otherSector));
    copiedSector = copiedSector->next;
    otherSector = otherSector->next;
  }
}

bool SectorMap::Contains(const Vector2f& point) const {
  float angle = Util::FindAngle(point - center);
  float depth = Util::Distance(point, center);
  return GetSectorDepth(angle) >= depth;
}

void SectorMap::Clear() {
  headSector->Clear();  
}

void SectorMap::Remove(Sector* sector) {
  sector->next->prev = sector->prev;
  sector->prev->next = sector->next;

  if (sector == headSector) {
    headSector = sector->next;
  }

  delete sector;
}


// TODO: make sure there are no remaining edge cases to handle.
// TODO: combine adjacent sectors of equal depth or otherwise minimize number
// of sectors.
void SectorMap::Add(float startAngle, float endAngle, float depth) {
  Sector* nextSector = nullptr;
  Sector* startSector = GetSector(startAngle);
  bool containsNewSector = Util::IsBetweenAngles(endAngle,
      startSector->StartAngle(), startSector->EndAngle());

  float originalDepth = startSector->depth;
  if (depth < originalDepth) {
    Sector* newSector = nullptr;
    if (startAngle != startSector->startAngle) {
      newSector = new Sector(startAngle, depth);
      startSector->PostInsert(newSector);
    } else {
      newSector = startSector;
      newSector->depth = depth;
    }

    // Creates a new sector representing the remainder of the start sector.
    if (containsNewSector) {
      newSector->PostInsert(new Sector(endAngle, originalDepth));
    } else {
      nextSector = newSector->next;
    }
  } else {
    nextSector = startSector->next;
  }

  if (!containsNewSector) {
    if (endAngle != nextSector->StartAngle()) {
      Add(nextSector->StartAngle(), endAngle, depth);
    }
  }
}

SectorMap::Sector* SectorMap::GetSector(float angle) {  
  Sector* sector = headSector;
  while (!sector->ContainsAngle(angle)) {
    sector = sector->next;
    assert(sector != headSector);
  }
  return sector;
}

// TODO: figure out how to merge with non-const version.
const SectorMap::Sector* SectorMap::GetSector(float angle) const {
  Sector* sector = headSector;
  while (!sector->ContainsAngle(angle)) {
    sector = sector->next;
    assert(sector != headSector);
  }
  return sector;
}

float SectorMap::GetSectorDepth(float angle) const {
  return GetSector(angle)->depth;
}