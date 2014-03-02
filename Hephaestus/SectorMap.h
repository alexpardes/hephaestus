#pragma once
#include "Vector2.h"
#include <map>
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
    class Sector;
    //void Remove(Sector* sector);
    Sector* GetSector(float angle);
    const Sector* GetSector(float angle) const;
    float GetSectorDepth(float angle) const;

    std::map<float, float> tree;
    Vector2f center;
};

class SectorMap::Sector {
  public:
    Sector(std::map<float, float>* tree) {
      this->tree = tree;
      tree->insert(std::pair<float, float>(0.f, FLT_MAX));
      leaf = tree->begin();
    }

    Sector(std::map<float, float>* tree, std::map<float, float>::iterator leaf) {
      this->tree = tree;
      this->leaf = leaf;
    }

    void PreInsert(float startAngle, float depth) {
      std::map<float, float>::iterator it;
      if (leaf == tree->begin() && startAngle > StartAngle()) {
        it = tree->end();
      } else {
        it = leaf;
      }

      tree->insert(it, std::pair<float, float>(startAngle, depth));
    }

    void PostInsert(float startAngle, float depth) {
      Next()->PreInsert(startAngle, depth);
    }

    float StartAngle() const { return leaf->first; }
    float EndAngle() const { return Next()->StartAngle(); }
    float Depth() const { return leaf->second; }

    void SetDepth(float depth) { leaf->second = depth; }

    bool ContainsAngle(float angle) const {
      return Util::IsBetweenAngles(angle, StartAngle(), EndAngle());
    }

    const Sector* Next() const {
      std::map<float, float>::iterator nextLeaf;
      if (std::next(leaf) == tree->end()) {
        nextLeaf = tree->begin();
      } else {
        nextLeaf = std::next(leaf);
      }

      return new Sector(tree, nextLeaf);
    }

    Sector* Next() {
      return const_cast<Sector*>(const_cast<const Sector*>(this)->Next());
    }

    const Sector* Prev() const {
      std::map<float, float>::iterator prevLeaf;
      if (leaf == tree->begin()) {
        prevLeaf = std::prev(tree->end());
      } else {
        prevLeaf = std::prev(leaf);
      }

      return new Sector(tree, prevLeaf);
    }

    Sector* Prev() {
      return const_cast<Sector*>(const_cast<const Sector*>(this)->Prev());
    }

  private:
    std::map<float, float>* tree;
    std::map<float, float>::iterator leaf;
};