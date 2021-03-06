#include "stdafx.h"
#include "Graphics.h"

void Graphics::DrawGame(const GameScene &scene,
                        float framerate,
                        float cycleRate) {
	window.clear();
  window.setView(gameInterface.MainView());
	DrawTerrain();
	DrawUnits(scene.Units());
	DrawProjectiles(scene.Projectiles());
  DrawFogOfWar(scene);

#if _DEBUG
  DrawDilation();
#endif

  DrawGameInterface(scene);

  //window.setView(gameInterface.MinimapView());
  //DrawTerrain();
  //DrawUnits(scene.units());
  //DrawProjectiles(scene.projectiles());

  DrawFramerate(framerate, cycleRate);

  window.setView(gameInterface.MainView());
	window.display();
}

void Graphics::DrawDilation() const {
  for (auto polygon : resourceManager.Walls()) {
    sf::VertexArray verts(sf::LinesStrip);
    for (auto v : polygon) {
      verts.append(sf::Vertex(v.Point(), sf::Color::Blue));
    }
    verts.append(sf::Vertex(polygon.begin().Point(), sf::Color::Blue));
    window.draw(verts);
  }

  for (auto polygon : resourceManager.DilatedWalls()) {
    sf::VertexArray verts(sf::LinesStrip);
    for (auto v : polygon) {
      verts.append(sf::Vertex(v.Point(), sf::Color::Red));
    }
    verts.append(sf::Vertex(polygon.begin().Point(), sf::Color::Red));
    window.draw(verts);
  }
}

void Graphics::DrawFogOfWar(const GameScene& scene) {
  auto &fogTexture = resourceManager.FogTexture();
  fogTexture.setActive();
  fogTexture.clear(sf::Color(0, 0, 0, 150));

  sf::VertexArray visibleArea(sf::Triangles);

  for (auto unit : scene.Units()) {
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
  window.draw(sf::Sprite(fogTexture.getTexture()));
}

void Graphics::DrawFramerate(float framerate, float cycleRate) const {
  window.setView(window.getDefaultView());
  sf::Font font = resourceManager.GetDefaultFont();

  std::string framerateString = std::to_string(int(framerate));
  sf::Text framerateDisplay(framerateString, font);
  framerateDisplay.setCharacterSize(15);
  framerateDisplay.setColor(sf::Color::White);
  float width = framerateDisplay.getLocalBounds().width;
  float left = window.getSize().x - width - 10;
  float top = 10;
  framerateDisplay.setPosition(left, top);
  window.draw(framerateDisplay);

  std::string cycleRateString = std::to_string(int(cycleRate));
  sf::Text cycleRateDisplay(cycleRateString, font);
  cycleRateDisplay.setCharacterSize(15);
  cycleRateDisplay.setColor(sf::Color::White);
  width = cycleRateDisplay.getLocalBounds().width;
  left = window.getSize().x - width - 10;
  top = 20 + framerateDisplay.getLocalBounds().height;
  cycleRateDisplay.setPosition(left, top);
  window.draw(cycleRateDisplay);
}

void Graphics::DrawTerrain() const {
  sf::Sprite mapSprite(resourceManager.GetMapImage());
  window.draw(mapSprite);
}

void Graphics::DrawUnitSprite(const UnitModel &unit, UnitImageType type) const {
  auto sprite = resourceManager.UnitSprite(unit.Type(), unit.Owner(), type);

  sprite.setPosition(unit.Position());

  auto rotation = type == kLower ? unit.Rotation() : unit.TorsoRotation();
  sprite.setRotation(Util::Degrees(rotation));
  if (unit.Facing() == kLeft)
    sprite.setScale(1, -1);

  window.draw(sprite);

}

void Graphics::DrawUnits(const std::list<UnitModel *> &units) const {
  for (UnitModel *unit : units) {
    if (gameInterface.Player() == unit->Owner() || unit->IsVisible()) {
      DrawUnitSprite(*unit, kLower);
      DrawUnitSprite(*unit, kUpper);
    }
	}
}

void Graphics::DrawProjectiles(const std::list<ProjectileModel*> &projectiles)
		const {
  for (auto projectile : projectiles) {
    if (projectile->Owner() != gameInterface.Player() && !projectile->IsVisible())
      continue;

    const sf::Texture &image =
        resourceManager.GetImage(projectile->Type());
		sf::Sprite projectile_sprite(image);
		Vector2f imageCenter(image.getSize().x * 0.5f, image.getSize().y * 0.5f);
		projectile_sprite.setOrigin(imageCenter);
		projectile_sprite.setPosition(projectile->Position());
    projectile_sprite.setRotation(Util::Degrees(projectile->Rotation()));
		window.draw(projectile_sprite);
	}
}

const float statBarWidth = 65.f;
const float statBarHeight = 5.f;
sf::RectangleShape StatBarOutline(const Vector2f &position) {
  auto statBarOutline = sf::RectangleShape(sf::Vector2f(statBarWidth, statBarHeight));
  statBarOutline.setOutlineColor(sf::Color::Black);
  statBarOutline.setOutlineThickness(1);
  statBarOutline.setFillColor(sf::Color::Transparent);
  statBarOutline.setOrigin(statBarWidth / 2, 0);
  statBarOutline.setPosition(position);
  return statBarOutline;
}

sf::RectangleShape StatBar(const Vector2f &position, float fraction, const sf::Color &color) {
  sf::RectangleShape statBar(sf::Vector2f(fraction * statBarWidth, statBarHeight));
  statBar.setOutlineColor(sf::Color::Transparent);
  statBar.setFillColor(color);
  statBar.setOrigin(statBarWidth / 2, 0);
  statBar.setPosition(position);
  return statBar;
}

void Graphics::DrawGameInterface(const GameScene &scene) const {
	sf::CircleShape selectionCircle;
	const std::list<UnitModel *> selectedUnits =
			gameInterface.GetSelectedUnits();

  for (UnitModel* unit : selectedUnits) {
		selectionCircle = sf::CircleShape(unit->Radius() + 1.f);
		selectionCircle.setPosition(unit->Position());
		selectionCircle.setOrigin(selectionCircle.getRadius(),
				selectionCircle.getRadius());
		selectionCircle.setFillColor(sf::Color(0, 0, 0, 0));
		selectionCircle.setOutlineColor(sf::Color(75, 255, 150, 150));
		selectionCircle.setOutlineThickness(1.9f);
		window.draw(selectionCircle);
	}

  for (UnitModel* unit : scene.Units()) {
    if (unit->Owner() == gameInterface.Player() || unit->IsVisible()) {
      float healthFraction = unit->CurrentHealth() / unit->MaxHealth();
      window.draw(StatBar(unit->Position() + Vector2f(0, unit->Radius()), healthFraction, sf::Color::Green));
      window.draw(StatBarOutline(unit->Position() + Vector2f(0, unit->Radius())));

      window.draw(StatBar(unit->Position() + Vector2f(0, unit->Radius() + statBarHeight + 1.f), unit->Stability(), sf::Color::Yellow));
      window.draw(StatBarOutline(unit->Position() + Vector2f(0, unit->Radius() + statBarHeight + 1.f)));
    }
  }

	if (gameInterface.is_selecting()) {
		window.draw(*gameInterface.GetSelectionBoxGraphic());
	}

  window.setView(window.getDefaultView());
	//window.draw(*gameInterface.GetInterfaceGrahic());

	//DrawMiniMap(scene);
}

void Graphics::DrawMiniMap(const GameScene &scene) const {
	Vector2f minimap_size(200.f, 200.f);
  for (UnitModel* unit : scene.Units()) {
		Vector2f position;
		position.x = unit->Position().x * minimap_size.x /
				resourceManager.MapSize().x;
		position.y = window.getSize().y - minimap_size.y +
				unit->Position().y * minimap_size.y /
				resourceManager.MapSize().y;
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
		window.draw(minimap_graphic);
	}
}