#pragma once

#include <SFML/System/Vector2.hpp>
#include <boost/functional/hash/hash.hpp>

typedef sf::Vector2f Vector2f;
typedef sf::Vector2i Vector2i;

struct Vector2iHash : std::unary_function<Vector2i, std::size_t> {
  std::size_t operator()(const Vector2i &t) const {
    std::size_t val = 0;
    boost::hash_combine(val, t.x);
    boost::hash_combine(val, t.y);
    return val;
  }
};