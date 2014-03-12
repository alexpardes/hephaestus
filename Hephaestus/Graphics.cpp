#include "stdafx.h"
#include "Graphics.h"

void Graphics::DrawGame(const GameScene &scene,
                        float framerate,
                        float cycleRate) {
	window_.clear();
  window_.setView(game_interface_.MainView());
	DrawTerrain();
	DrawUnits(scene.units());
	DrawProjectiles(scene.projectiles());

  DrawFogOfWar(scene);

  DrawGameInterface(scene);

  window_.setView(game_interface_.MinimapView());
  DrawTerrain();
  DrawUnits(scene.units());
  DrawProjectiles(scene.projectiles());

  DrawFramerate(framerate, cycleRate);


  window_.setView(game_interface_.MainView());
	window_.display();
}

std::vector<sf::Vertex> Graphics::TessellateSector(SectorMap::Sector& sector) {
  std::vector<sf::Vertex> result;

  float depth = std::min(sector.Depth(), 100000.f);

  float startAngle = sector.StartAngle();
  float endAngle = sector.EndAngle();
  if (endAngle == startAngle || Util::AngleCCW(startAngle, endAngle) > M_PI / 2) {
    endAngle = startAngle + M_PI / 2;
  }

  do {
    //sf::ConvexShape visionTriangle(3);

    Vector2f direction1 = Util::MakeUnitVector(startAngle);
    Vector2f direction2 = Util::MakeUnitVector(endAngle);
    Vector2f point1 = direction1 * depth;
    Vector2f point2 = direction2 * depth;

    result.push_back(sf::Vertex(Vector2f(0, 0)));
    result.push_back(sf::Vertex(point1));
    result.push_back(sf::Vertex(point2));

    //visionTriangle.setPoint(0, Vector2f(0, 0));
    //visionTriangle.setPoint(1, point1);
    //visionTriangle.setPoint(2, point2);

    //result.push_back(visionTriangle);

    startAngle = endAngle;
    endAngle = sector.EndAngle();
    if (endAngle == startAngle || Util::AngleCCW(startAngle, endAngle) > M_PI / 2) {
      endAngle = startAngle + M_PI / 2;
    }
  } while (startAngle != sector.EndAngle());

  return result;
}

void Graphics::DrawFogOfWar(const GameScene& scene) {
  sf::RenderTexture& fogTexture = resource_manager_.FogTexture();
  fogTexture.clear(sf::Color(0, 0, 0, 150));

  sf::VertexArray visibleArea(sf::Triangles);

  for (const SectorMap* unitView : scene.UnitViews()) {
    std::shared_ptr<SectorMap::Sector> startSector = unitView->GetSector(0);
    std::shared_ptr<SectorMap::Sector> sector = startSector;
    do {
      for (sf::Vertex visionVertex : TessellateSector(*sector)) {
        visionVertex.color = sf::Color::Transparent;
        visionVertex.position += unitView->Center();
        visibleArea.append(visionVertex);
      }

      sector = sector->Next();
    } while (*sector != *startSector);
  }

  fogTexture.draw(visibleArea, sf::BlendNone);

  fogTexture.display();
  window_.draw(sf::Sprite(fogTexture.getTexture()));

  //for (int x = 0; x < MapSize().x; ++x) {
  //  for (int y = 0; y < MapSize().y; ++y) {
  //    if (scene.IsVisible(x, y)) {
  //      resource_manager_.FogOfWar().SetTile(x, y, Vector2i(0, 0));
  //    } else {
  //      resource_manager_.FogOfWar().SetTile(x, y, Vector2i(1, 0));
  //    }
  //  }
  //}
  //window_.draw(resource_manager_.FogOfWar());
}

void Graphics::DrawFramerate(float framerate, float cycleRate) const {
  window_.setView(window_.getDefaultView());
  sf::Font font = resource_manager_.GetDefaultFont();

  std::string framerateString = std::to_string(int(framerate));
  sf::Text framerateDisplay(framerateString, font);
  framerateDisplay.setCharacterSize(15);
  framerateDisplay.setColor(sf::Color::White);
  float width = framerateDisplay.getLocalBounds().width;
  float left = window_.getSize().x - width - 10;
  float top = 10;
  framerateDisplay.setPosition(left, top);
  window_.draw(framerateDisplay);

  std::string cycleRateString = std::to_string(int(cycleRate));
  sf::Text cycleRateDisplay(cycleRateString, font);
  cycleRateDisplay.setCharacterSize(15);
  cycleRateDisplay.setColor(sf::Color::White);
  width = cycleRateDisplay.getLocalBounds().width;
  left = window_.getSize().x - width - 10;
  top = 20 + framerateDisplay.getLocalBounds().height;
  cycleRateDisplay.setPosition(left, top);
  window_.draw(cycleRateDisplay);
}

void Graphics::DrawTerrain() const {
  sf::Sprite mapSprite(resource_manager_.GetMapImage());
  window_.draw(mapSprite);
}

void Graphics::DrawUnits(const std::list<UnitModel *> &units) const {
  for (UnitModel* unit : units) {
    if (unit->IsVisible()) {
		  const sf::Texture &unit_image = resource_manager_.GetImage(unit->Name(),
				  unit->Owner());
		  sf::Sprite unit_sprite(unit_image);
		  Vector2f image_center(unit_image.getSize().x * 0.5,
          unit_image.getSize().y * 0.5);
		  unit_sprite.setOrigin(image_center);
		  unit_sprite.setPosition(unit->position());
		  unit_sprite.setRotation(Util::Degrees(unit->rotation()));
		  window_.draw(unit_sprite);
    }
	}
}

void Graphics::DrawProjectiles(const std::list<ProjectileModel *> &projectiles)
		const {
  for (ProjectileModel* projectile : projectiles) {
    const sf::Texture &image =
        resource_manager_.GetImage(projectile->Name());
		sf::Sprite projectile_sprite(image);
		Vector2f imageCenter(image.getSize().x * 0.5, image.getSize().y * 0.5);
		projectile_sprite.setOrigin(imageCenter);
		projectile_sprite.setPosition(projectile->position());
    projectile_sprite.setRotation(Util::Degrees(projectile->Rotation()));
		window_.draw(projectile_sprite);
	}
}

void Graphics::DrawGameInterface(const GameScene &scene) const {
	sf::CircleShape selection_circle;
	const std::list<UnitModel *> selected_units =
			game_interface_.GetSelectedUnits();
  for (UnitModel* unit : selected_units) {
		selection_circle = sf::CircleShape(unit->radius() + 1.f);
		selection_circle.setPosition(unit->position());
		selection_circle.setOrigin(selection_circle.getRadius(),
				selection_circle.getRadius());
		selection_circle.setFillColor(sf::Color(0, 0, 0, 0));
		selection_circle.setOutlineColor(sf::Color(75, 255, 150, 150));
		selection_circle.setOutlineThickness(1.9f);
		window_.draw(selection_circle);
	}
	if (game_interface_.is_selecting()) {
		window_.draw(*game_interface_.GetSelectionBoxGraphic());
	}
  
  //Vector2i mapSize = resource_manager_.MapSize();
  //sf::Color fogColor(100, 100, 100, 50);
  //sf::RectangleShape fogTile(Vector2f(kTileSize, kTileSize));
  //fogTile.setFillColor(fogColor);
  //for (int x = 0; x < mapSize.x; ++x) {
  //  for (int y = 0; y < mapSize.y; ++y) {
  //    Vector2f position(x*kTileSize, y*kTileSize);
  //    if (!scene.IsVisible(x, y)) {
  //      fogTile.setPosition(position);
  //      //window_.draw(fogTile);
  //    }
  //  }
  //}

  window_.setView(window_.getDefaultView());
	window_.draw(*game_interface_.GetInterfaceGrahic());

	//DrawMiniMap(scene);
}

void Graphics::DrawMiniMap(const GameScene &scene) const {
	Vector2f minimap_size(200.f, 200.f);
  for (UnitModel* unit : scene.units()) {
		Vector2f position;
		position.x = unit->position().x * minimap_size.x /
				resource_manager_.MapSize().x / kTileSize;
		position.y = window_.getSize().y - minimap_size.y +
				unit->position().y * minimap_size.y /
				resource_manager_.MapSize().y / kTileSize;
		sf::CircleShape minimap_graphic(3.f);
		minimap_graphic.setPosition(position);
		switch (unit->Owner()) {
			case kPlayer1:
				minimap_graphic.setFillColor(sf::Color(255, 0, 0, 255));
				break;
			case kPlayer2:
				minimap_graphic.setFillColor(sf::Color(0, 0, 255, 255));
				break;
		}
		window_.draw(minimap_graphic);
	}
}