#include "stdafx.h"
#include "Poly.h"
#include "Util.h"

typedef Poly::Vertex Vertex;

Poly::Poly() {
  isReversed = false;
}

size_t Poly::Size() const {
  return vertices.size();
}

void Poly::Add(const Vector2f &vertex) {
  vertices.push_back(vertex);
}

Vertex Poly::begin() const {
  return Vertex(*this, 0);
}

Vertex Poly::end() const {
  return Vertex(*this, Size());
}

bool Poly::IsReversed() const {
  return isReversed;
}

void Poly::SetReversed(bool reversed) {
  isReversed = reversed;
}

bool Poly::Contains(const Vector2f &point) const {
  LineSegment ray(Vector2f(-10000, -10000), point);
  int nIntersections = 0;
  for (auto vertex : *this) {
    if (ray.Intersects(vertex.Segment()))
      ++nIntersections;
  }
  return (nIntersections % 2 == 1) ^ isReversed;
}

Vertex::Vertex() {
  this->polygon = nullptr;
}

Vertex::Vertex(const Poly &polygon, int index) {
  this->polygon = &polygon;
  this->index = index;
}

Vector2f Vertex::Point() const {
  return polygon->vertices[index];
}

Vertex Vertex::Next() const {
  return Vertex(*polygon, (polygon->Size() + index + 1) % polygon->Size());
}

Vertex Vertex::Prev() const {
  return Vertex(*polygon, (polygon->Size() + index - 1) % polygon->Size());
}

Vertex Vertex::operator++() {
  ++index;
  return *this;
}

Vertex Vertex::operator++(int) {
  return Vertex(*polygon, index++);
}

bool Vertex::operator==(const Vertex &other) const {
  return index == other.index && polygon == other.polygon;
}

bool Vertex::operator!=(const Vertex &other) const {
  return !(*this == other);
}

Vertex Vertex::operator*() const {
  return *this;
}

float Vertex::InteriorAngle() const {
  Vector2f v1 = Util::Normalized(Prev().Point() - Point());
  Vector2f v2 = Util::Normalized(Next().Point() - Point());
  return Util::FindAngle(v2, v1);
}

bool Vertex::IsConvex() const {
  return InteriorAngle() < M_PI;
}


Vector2f Vertex::Normal() const {
  Vector2f v1 = Util::Normalized(Prev().Point() - Point());
  Vector2f v2 = Util::Normalized(Next().Point() - Point());
  return Util::Normalized(Util::Perpendicular(v1 - v2));
}

LineSegment Vertex::Segment() const {
  return LineSegment(Point(), Next().Point());
}