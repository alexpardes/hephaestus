#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "GameInterface.h"
#include "ResourceManager.h"
#include "Util.h"

class Graphics {
	public:
		Graphics(sf::RenderWindow &window,
				const GameInterface &game_interface,
				const ResourceManager &resource_manager) :
				window_(window), game_interface_(game_interface),
				resource_manager_(resource_manager) {
    }
		void DrawGame(const GameScene &scene) const;
	private:
		sf::RenderWindow &window_;
		const ResourceManager &resource_manager_;
		const GameInterface &game_interface_;
		void DrawTerrain() const;
		void DrawUnits(const std::list<UnitModel *> &units) const;
		void DrawProjectiles(const std::list<ProjectileModel *> &projectiles) const;
		void DrawGameInterface(const GameScene& scene) const;
    void DrawSelectionGraphics(const GameScene& scene) const;
    void DrawVisibilityGraphics(const GameScene& scene) const;
		void Graphics::DrawMiniMap(const GameScene &scene) const;
};