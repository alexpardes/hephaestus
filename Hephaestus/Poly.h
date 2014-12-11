#pragma once
#include <vector>
#include "Util.h"
#include "Vector2.h"
#include "LineSegment.h"

class Poly {
  public:
    class Vertex;
    Poly();
    Vertex begin() const;
    Vertex end() const;
    size_t Size() const;
    bool Contains(const Vector2f &point) const;
    void Add(const Vector2f &vertex);
    bool IsReversed() const;
    void SetReversed(bool reversed);

  private:
    friend class Vertex;

    // Assumed to be in CW order if reversed, CCW otherwise.
    std::vector<const Vector2f> vertices;
    bool isReversed; // True iff the polygon represents the area geometrically outside the shape.
};

class Poly::Vertex {
  public:
    Vertex();
    Vector2f Point() const;
    float InteriorAngle() const;
    bool IsConvex() const;
    Vector2f Normal() const;
    LineSegment Segment() const;
    Vertex Next() const;
    Vertex Prev() const;
    Vertex operator++();
    Vertex operator++(int);
    Vertex operator*() const;
    bool operator==(const Vertex &other) const;
    bool operator!=(const Vertex &other) const;

  private:
    friend class Poly;
    friend struct std::hash<Vertex>;
    Vertex(const Poly &polygon, int index);

    const Poly *polygon;
    int index;
};

namespace std {
  template <>
  struct hash<Poly::Vertex> {
    std::size_t operator()(const Poly::Vertex &v) const {
      auto hash = Util::HashStart();
      Util::Hash(hash, v.polygon);
      Util::Hash(hash, v.index);
      return hash;
    }
  };
}