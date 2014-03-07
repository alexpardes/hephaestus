#pragma once
#include "Vector2.h"
#include <SFML/Graphics.hpp>

class TileMap : public sf::Drawable, public sf::Transformable {
  public:
    TileMap(const Vector2i& size, float tileSize);
    void SetTileset(const sf::Texture& tileset);
    void SetTile(int x, int y, const Vector2i& texturePosition);

  private:
    std::vector<sf::Vertex*> GetTileVertices(int x, int y);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    sf::VertexArray vertices;
    sf::Texture tileset;
    float tileSize;
    Vector2i size;
};