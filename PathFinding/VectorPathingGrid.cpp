#include "VectorPathingGrid.h"

VectorPathingGrid::VectorPathingGrid(const Vector2i &size) {
  Construct(size);
}

VectorPathingGrid::VectorPathingGrid(int width, int height) {
  Construct(Vector2i(width, height));
}

void VectorPathingGrid::Construct(const Vector2i &size) {
  isBlocked = std::vector<std::vector<bool>>(size.x);
  for (int x = 0; x < size.x; ++x) {
    isBlocked[x] = std::vector<bool>(size.y);
    //isBlocked.push_back(std::vector<bool>(size.y));
    //while (isBlocked.back().size() < size.y) {
    //  isBlocked.back().push_back(false);
    //}
  }
}

bool VectorPathingGrid::IsBlocked(const Vector2i &tile) const {
  return Contains(tile) && isBlocked[tile.x][tile.y];
}

bool VectorPathingGrid::IsUnblocked(const Vector2i &tile) const {
  return Contains(tile) && !isBlocked[tile.x][tile.y];
}

bool VectorPathingGrid::Contains(const Vector2i &tile) const {
  return tile.x >= 0 && tile.x < Size().x && tile.y >= 0 && tile.y < Size().y;
}

void VectorPathingGrid::SetBlocked(const Vector2i &tile, bool isBlocked) {
  this->isBlocked[tile.x][tile.y] = isBlocked;
}

Vector2i VectorPathingGrid::Size() const {
  return Vector2i(isBlocked.size(), isBlocked.front().size());
}