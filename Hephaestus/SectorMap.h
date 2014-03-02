#pragma once
#include "Vector2.h"
#include <list>
#include "Util.h"

class SectorMap {
  public:
    SectorMap();
    SectorMap(const Vector2f& center);
    SectorMap(const SectorMap& map);

    // Returns true iff the given point is less than the max distance from the
    // center given for its sector.
    bool Contains(const Vector2f& point) const;
    void Add(float startAngle, float endAngle, float distance);
    void Clear();
    void SetCenter(const Vector2f& center) { this->center = center; }

  private:
    struct Sector;
    void Remove(Sector* sector);
    Sector* GetSector(float angle);
    const Sector* GetSector(float angle) const;
    float GetSectorDepth(float angle) const;

    // Arbitrary start element in circular list.
    // TODO: use a tree instead of a linked list.
    Sector* headSector;
    Vector2f center;
};

struct SectorMap::Sector {
    Sector(float startAngle, float depth) :
        startAngle(startAngle), depth(depth) {    
      next = this;
      prev = this;
    }
    
    Sector(const Sector& sector) {
      startAngle = sector.startAngle;
      depth = sector.depth;
      next = this;
      prev = this;
    }

    float StartAngle() const { return startAngle; }
    float EndAngle() const { return next->startAngle; }

    bool ContainsAngle(float angle) const {
      return Util::IsBetweenAngles(angle, StartAngle(), EndAngle());
    }

    void Clear() {
      prev = this;
      next = this;
      depth = FLT_MAX;
    }

    void PreInsert(Sector* sector) {
      sector->prev = prev;
      sector->next = this;
      prev->next = sector;
      prev = sector;
    }

    void PostInsert(Sector* sector) {
      next->PreInsert(sector);
    }

    bool IsContainedIn(float angle1, float angle2) const {
      return Util::IsBetweenAngles(startAngle, angle1, angle2)
          && Util::IsBetweenAngles(EndAngle(), angle1, angle2);
    }

    Sector* prev;
    Sector* next;
    float startAngle, depth;
};