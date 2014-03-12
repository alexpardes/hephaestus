#pragma once
#include "Vector2.h"
#include <map>
#include "Util.h"

class SectorMap {
  public:
    class Sector;

    SectorMap();
    SectorMap(const Vector2f& center);
    SectorMap(const SectorMap& map);

    // Returns true iff the given point is less than the max distance from the
    // center given for its sector.
    bool Contains(const Vector2f& point) const;

    // Currently this method is only an approximation.
    bool IntersectsCircle(const Vector2f& center, float radius) const;

    void Add(float startAngle, float endAngle, float distance);
    void Clear();
    Vector2f Center() const { return center; }
    void SetCenter(const Vector2f& center) { this->center = center; }

    const std::shared_ptr<Sector> GetSector(float angle) const;

  private:
    void Add(std::shared_ptr<Sector> startSector, float startAngle, float endAngle, float depth);
    std::shared_ptr<Sector> GetSector(float angle);
    float GetSectorDepth(float angle) const;

    std::map<float, float> tree;
    Vector2f center;
};

class SectorMap::Sector {
  public:
    Sector(std::map<float, float>& tree) : tree(tree) {
      tree.insert(std::pair<float, float>(0.f, FLT_MAX));
      leaf = tree.begin();
    }

    Sector(std::map<float, float>& tree,
        std::map<float, float>::iterator leaf) : tree(tree) {      
      this->leaf = leaf;
    }

    bool Sector::operator==(const Sector& other) {
      return leaf == other.leaf && tree == other.tree;
    }

    bool Sector::operator!=(const Sector& other) {
      return !(*this == other);
    }

    void PreInsert(float startAngle, float depth) {
      std::map<float, float>::iterator it;
      if (leaf == tree.begin() && startAngle > StartAngle()) {
        it = tree.end();
      } else {
        it = leaf;
      }

      tree.insert(it, std::pair<float, float>(startAngle, depth));
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

    const std::shared_ptr<Sector> Next() const {
      std::map<float, float>::iterator nextLeaf;
      if (std::next(leaf) == tree.end()) {
        nextLeaf = tree.begin();
      } else {
        nextLeaf = std::next(leaf);
      }

      return std::make_shared<Sector>(tree, nextLeaf);
    }

    std::shared_ptr<Sector> Next() {
      return const_cast<const Sector*>(this)->Next();
    }

    const std::shared_ptr<Sector> Prev() const {
      std::map<float, float>::iterator prevLeaf;
      if (leaf == tree.begin()) {
        prevLeaf = std::prev(tree.end());
      } else {
        prevLeaf = std::prev(leaf);
      }

      return std::make_shared<Sector>(tree, prevLeaf);
    }

    std::shared_ptr<Sector> Prev() {
      return const_cast<const Sector*>(this)->Prev();
    }

  private:
    std::map<float, float>& tree;
    std::map<float, float>::iterator leaf;
};