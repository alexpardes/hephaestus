#include "stdafx.h"
#include "Graphics.h"

void Graphics::DrawGame(const GameScene &scene,
                        float framerate,
                        float cycleRate) {
	window_.clear();
  window_.setView(gameInterface.MainView());
	DrawTerrain();
	DrawUnits(scene.units());
	DrawProjectiles(scene.projectiles());

  DrawFogOfWar(scene);

  DrawGameInterface(scene);

  window_.setView(gameInterface.MinimapView());
  DrawTerrain();
  DrawUnits(scene.units());
  DrawProjectiles(scene.projectiles());

  DrawFramerate(framerate, cycleRate);


  window_.setView(gameInterface.MainView());
	window_.display();
}

void Graphics::DrawFogOfWar(const GameScene& scene) {
  sf::RenderTexture& fogTexture = resource_manager_.FogTexture();
  fogTexture.clear(sf::Color(0, 0, 0, 150));

  sf::VertexArray visibleArea(sf::Triangles);

  for (UnitModel* unit : scene.units()) {
    if (unit->Owner() == gameInterface.Player()) {
      for (size_t i = 0; i < unit->Visibility().VisiblePoints().size(); i += 2) {
        sf::Vertex p1(unit->Visibility().Origin());
        sf::Vertex p2(unit->Visibility().VisiblePoints()[i]);
        sf::Vertex p3(unit->Visibility().VisiblePoints()[i + 1]);
        visibleArea.append(p1);
        visibleArea.append(p2);
        visibleArea.append(p3);
      }
    }
  }

  for (size_t i = 0; i < visibleArea.getVertexCount(); ++i) {
    visibleArea[i].color = sf::Color::Transparent;
  }

  fogTexture.draw(visibleArea, sf::BlendNone);
  fogTexture.display();
  window_.draw(sf::Sprite(fogTexture.getTexture()));
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
		  Vector2f image_center(unit_image.getSize().x * 0.5f,
          unit_image.getSize().y * 0.5f);
		  unit_sprite.setOrigin(image_center);
		  unit_sprite.setPosition(unit->position());
		  unit_sprite.setRotation(Util::Degrees(unit->rotation()));
      if (unit->Facing() == kLeft)
        unit_sprite.setScale(1, -1);

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
		Vector2f imageCenter(image.getSize().x * 0.5f, image.getSize().y * 0.5f);
		projectile_sprite.setOrigin(imageCenter);
		projectile_sprite.setPosition(projectile->position());
    projectile_sprite.setRotation(Util::Degrees(projectile->Rotation()));
		window_.draw(projectile_sprite);
	}
}

void Graphics::DrawGameInterface(const GameScene &scene) const {
	sf::CircleShape selectionCircle;
	const std::list<UnitModel *> selectedUnits =
			gameInterface.GetSelectedUnits();
  for (UnitModel* unit : selectedUnits) {
		selectionCircle = sf::CircleShape(unit->Radius() + 1.f);
		selectionCircle.setPosition(unit->position());
		selectionCircle.setOrigin(selectionCircle.getRadius(),
				selectionCircle.getRadius());
		selectionCircle.setFillColor(sf::Color(0, 0, 0, 0));
		selectionCircle.setOutlineColor(sf::Color(75, 255, 150, 150));
		selectionCircle.setOutlineThickness(1.9f);
		window_.draw(selectionCircle);
	}

  for (UnitModel* unit : scene.units()) {
    if (unit->IsVisible()) {
      float healthBarWidth = 65.f;
      float healthBarHeight = 5.f;
      sf::RectangleShape healthBarOutline(sf::Vector2f(healthBarWidth, healthBarHeight));
      healthBarOutline.setOrigin(healthBarWidth / 2, -unit->Radius());
      healthBarOutline.setPosition(unit->position());
      healthBarOutline.setOutlineColor(sf::Color::Black);
      healthBarOutline.setOutlineThickness(1);
      healthBarOutline.setFillColor(sf::Color::Transparent);

      float healthFraction = unit->CurrentHealth() / unit->MaxHealth();
      sf::RectangleShape healthBar(sf::Vector2f(healthFraction * healthBarWidth, healthBarHeight));
      healthBar.setOutlineColor(sf::Color::Transparent);
      healthBar.setFillColor(sf::Color::Green);
      healthBar.setOrigin(healthBarOutline.getOrigin());
      healthBar.setPosition(healthBarOutline.getPosition());
      window_.draw(healthBar);
      window_.draw(healthBarOutline);
    }
  }

	if (gameInterface.is_selecting()) {
		window_.draw(*gameInterface.GetSelectionBoxGraphic());
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
	window_.draw(*gameInterface.GetInterfaceGrahic());

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