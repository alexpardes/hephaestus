#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "GameInterface.h"
#include "ResourceManager.h"
#include "Util.h"
#include "SectorMap.h"

class Graphics {
	public:
		Graphics(sf::RenderWindow &window,
				const GameInterface &gameInterface,
				ResourceManager &resourceManager) :
				window(window), gameInterface(gameInterface),
				resourceManager(resourceManager) {
    }
		void DrawGame(const GameScene &scene, float framerate, float cycleRate);
	private:
    Vector2i MapSize() const { return resourceManager.MapSize(); }
		sf::RenderWindow &window;
		ResourceManager &resourceManager;
		const GameInterface &gameInterface;
    void DrawFogOfWar(const GameScene& scene);
    void DrawFramerate(float framerate, float cycleRate) const;
		void DrawTerrain() const;
		void DrawUnits(const std::list<UnitModel *> &units) const;
    void DrawUnitSprite(const UnitModel &unit, UnitImageType type) const;
		void DrawProjectiles(const std::list<ProjectileModel *> &projectiles) const;
		void DrawGameInterface(const GameScene& scene) const;
    void DrawSelectionGraphics(const GameScene& scene) const;
    void DrawVisibilityGraphics(const GameScene& scene) const;
		void DrawMiniMap(const GameScene &scene) const;
    void DrawDilation() const;
};