#include "stdafx.h"
#include "TileMap.h"

TileMap::TileMap(const Vector2i& size, float tileSize) {
  this->tileSize = tileSize;
  this->size = size;
  vertices.setPrimitiveType(sf::Quads);
  vertices.resize(4 * size.x * size.y);
  for (int x = 0; x < size.x; ++x) {
    for (int y = 0; y < size.y; ++y) {
      std::vector<sf::Vertex*> vertices = GetTileVertices(x, y);

      vertices[0]->position = sf::Vector2f(x * tileSize, y * tileSize);
      vertices[1]->position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
      vertices[2]->position = sf::Vector2f((x + 1) * tileSize,
          (y + 1) * tileSize);
      vertices[3]->position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);

      SetTile(x, y, Vector2i(0, 0));
    }
  }
}

std::vector<sf::Vertex*> TileMap::GetTileVertices(int x, int y) {
  std::vector<sf::Vertex*> result;
  sf::Vertex* firstVertex = &vertices[(x + y * size.x) * 4];
  for (int i = 0; i < 4; ++i) {
    result.push_back(firstVertex + i);
  }
  return result;
}

void TileMap::SetTileset(const sf::Texture& tileset) {
  this->tileset = tileset;
}

void TileMap::SetTile(int x, int y, const Vector2i& texturePosition) {
  std::vector<sf::Vertex*> vertices = GetTileVertices(x, y);
  vertices[0]->texCoords = sf::Vector2f(texturePosition.x * tileSize,
      texturePosition.y * tileSize);
  vertices[1]->texCoords = sf::Vector2f((texturePosition.x + 1) * tileSize,
      texturePosition.y * tileSize);
  vertices[2]->texCoords = sf::Vector2f((texturePosition.x + 1) * tileSize,
      (texturePosition.y + 1) * tileSize);
  vertices[3]->texCoords = sf::Vector2f(texturePosition.x * tileSize,
      (texturePosition.y + 1) * tileSize);
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.texture = &tileset;
  states.transform *= getTransform();
  target.draw(vertices);
}