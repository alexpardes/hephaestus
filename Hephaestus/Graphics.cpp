#include "stdafx.h"
#include "Graphics.h"

void Graphics::DrawGame(const GameScene &scene) const {
	window_.Clear(sf::Color(255, 255, 255));
	DrawTerrain();
	DrawUnits(scene.units());
	DrawProjectiles(scene.projectiles());
	DrawGameInterface(scene);
	window_.Display();
}

void Graphics::DrawTerrain() const {
	std::vector<std::vector<terrainId>> terrain =
			resource_manager_.GetTerrain();
	for (int i = 0; i < terrain.size(); ++i) {
		for (int j = 0; j < terrain[i].size(); ++j) {
			const sf::Image &terrain_image =
					resource_manager_.GetImage(terrain[i][j]);
			sf::Sprite terrain_sprite(terrain_image);
			terrain_sprite.SetPosition(Vector2f(i*kTileSize, j*kTileSize) -
					game_interface_.screen_position());
			window_.Draw(terrain_sprite);
		}
	}
}

void Graphics::DrawUnits(const std::list<UnitModel *> &units) const {
	for (std::list<UnitModel *>::const_iterator unit_iterator = units.begin();
			unit_iterator != units.end(); ++unit_iterator) {
		UnitModel &unit = **unit_iterator;
		const sf::Image &unit_image = resource_manager_.GetImage(unit.name(),
				unit.owner());
		sf::Sprite unit_sprite(unit_image);
		unit_sprite.SetCenter(Vector2f(30.f, 30.f));
		unit_sprite.SetPosition(unit.position() -
				game_interface_.screen_position());
		unit_sprite.SetRotation(-1.f *
				Util::Degrees(unit.rotation()));
		window_.Draw(unit_sprite);
	}
}

void Graphics::DrawProjectiles(const std::list<ProjectileModel *> &projectiles)
		const {
	for (std::list<ProjectileModel *>::const_iterator projectile =
			projectiles.begin(); projectile != projectiles.end();
			++projectile) {
		sf::Sprite projectile_sprite((*projectile)->image());
		projectile_sprite.SetCenter(Vector2f(3.f, 3.f));
		projectile_sprite.SetPosition((*projectile)->position() -
				game_interface_.screen_position());
		window_.Draw(projectile_sprite);
	}
}

void Graphics::DrawGameInterface(const GameScene &scene) const {
	sf::Shape selection_circle;
	const std::list<UnitModel *> selected_units =
			game_interface_.GetSelectedUnits();
	for (std::list<UnitModel *>::const_iterator unit_iterator =
			selected_units.begin(); unit_iterator != selected_units.end();
			++unit_iterator) {
		UnitModel &unit = **unit_iterator;
		selection_circle = sf::Shape::Circle(unit.position() -
				game_interface_.screen_position(),
				unit.radius() + 1.f,
				sf::Color(0, 0, 0, 0),
				1.9f, sf::Color(75, 255, 150, 150));
		window_.Draw(selection_circle);
	}
	if (game_interface_.is_selecting()) {
		window_.Draw(game_interface_.GetSelectionBoxGraphic());
	}
	window_.Draw(game_interface_.GetInterfaceGrahic());
	DrawMiniMap(scene);
}

void Graphics::DrawMiniMap(const GameScene &scene) const {
	Vector2f minimap_size(200.f, 200.f);
	for (std::list<UnitModel *>::const_iterator unit_iterator =
			scene.units().begin(); unit_iterator != scene.units().end();
			++unit_iterator) {
		UnitModel &unit = **unit_iterator;
		Vector2f position;
		position.x = unit.position().x * minimap_size.x /
				resource_manager_.GetMapSize().x / kTileSize;
		position.y = window_.GetHeight() - minimap_size.y +
				unit.position().y * minimap_size.y /
				resource_manager_.GetMapSize().y / kTileSize;
		sf::Shape minimap_graphic;
		switch (unit.owner()) {
			case kPlayer1:
				minimap_graphic = sf::Shape::Circle(position, 3.f,
						sf::Color(255, 0, 0, 255));
				break;
			case kPlayer2:
				minimap_graphic = sf::Shape::Circle(position, 3.f,
						sf::Color(0, 0, 255, 255));
				break;
		}
		window_.Draw(minimap_graphic);
	}
}