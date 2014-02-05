#include "stdafx.h"
#include "Graphics.h"

void Graphics::DrawGame(const GameScene &scene) const {
	window_.clear(sf::Color(255, 255, 255));
	DrawTerrain();
	DrawUnits(scene.units());
	DrawProjectiles(scene.projectiles());
	DrawGameInterface(scene);
	window_.display();
}

void Graphics::DrawTerrain() const {
	std::vector<std::vector<TerrainId>> terrain =
			resource_manager_.GetTerrain();
	for (int i = 0; i < terrain.size(); ++i) {
		for (int j = 0; j < terrain[i].size(); ++j) {
			const sf::Texture &terrain_image =
					resource_manager_.GetImage(terrain[i][j]);
			//sf::Sprite terrain_sprite(terrain_image);
			sf::Sprite terrain_sprite;
			terrain_sprite.setTexture(terrain_image);
			Vector2f position = Vector2f(i*kTileSize, j*kTileSize) -
					game_interface_.screen_position();

			terrain_sprite.setPosition(position);
			window_.draw(terrain_sprite);
		}
	}
}

void Graphics::DrawUnits(const std::list<UnitModel *> &units) const {
	for (std::list<UnitModel *>::const_iterator unit_iterator = units.begin();
			unit_iterator != units.end(); ++unit_iterator) {
		UnitModel &unit = **unit_iterator;
		const sf::Texture &unit_image = resource_manager_.GetImage(unit.Name(),
				unit.owner());
		sf::Sprite unit_sprite(unit_image);
		Vector2f image_center(unit_image.getSize().x * 0.5,
        unit_image.getSize().y * 0.5);
		unit_sprite.setOrigin(image_center);
		unit_sprite.setPosition(unit.position() -
				game_interface_.screen_position());
		unit_sprite.setRotation(Util::Degrees(unit.rotation()));
		window_.draw(unit_sprite);
	}
}

void Graphics::DrawProjectiles(const std::list<ProjectileModel *> &projectiles)
		const {
	for (std::list<ProjectileModel *>::const_iterator projectile =
			projectiles.begin(); projectile != projectiles.end();
			++projectile) {
    const sf::Texture &image =
        resource_manager_.GetImage((*projectile)->Name());
		sf::Sprite projectile_sprite(image);
		Vector2f imageCenter(image.getSize().x * 0.5, image.getSize().y * 0.5);
		projectile_sprite.setOrigin(imageCenter);
		projectile_sprite.setPosition((*projectile)->position() -
				game_interface_.screen_position());
    projectile_sprite.setRotation(Util::Degrees((*projectile)->Rotation()));
		window_.draw(projectile_sprite);
	}
}

void Graphics::DrawGameInterface(const GameScene &scene) const {
	sf::CircleShape selection_circle;
	const std::list<UnitModel *> selected_units =
			game_interface_.GetSelectedUnits();
	for (std::list<UnitModel *>::const_iterator unit_iterator =
			selected_units.begin(); unit_iterator != selected_units.end();
			++unit_iterator) {
		UnitModel &unit = **unit_iterator;
		selection_circle = sf::CircleShape(unit.radius() + 1.f);
		selection_circle.setPosition(unit.position() -
				game_interface_.screen_position());
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
	window_.draw(*game_interface_.GetInterfaceGrahic());
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
		position.y = window_.getSize().y - minimap_size.y +
				unit.position().y * minimap_size.y /
				resource_manager_.GetMapSize().y / kTileSize;
		sf::CircleShape minimap_graphic(3.f);
		minimap_graphic.setPosition(position);
		switch (unit.owner()) {
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