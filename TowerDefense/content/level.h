#pragma once

#include "placementarea.h"
#include "turret.h"
#include "graphics/scene.h"

namespace TowerDefense
{
	namespace content
	{
		struct GameResources
		{
			gfx::Model::P   turretModel;
			gfx::Texture::P turretTexture;
			gfx::Texture::P turretNormalmap;
			phy::Hitbox::P  turretHitbox;
			
			gfx::Model::P   enemyModel;
			gfx::Texture::P enemyTexture;
			gfx::Texture::P enemyNormalmap;
			phy::Hitbox::P  enemyHitbox;

			gfx::Model::P   areaModel;
			gfx::Texture::P areaTexture;
			gfx::Texture::P areaNormalmap;
			phy::Hitbox::P  areaHitbox;

			GameResources(gfx::Graphics& graphics);
		};

		class Level
		{
		public:
			SMART_PTR(Level)
		private:
			int m_width;
			int m_height;
			std::vector<PlacementArea::P> m_places;
			std::vector<Turret::P> m_turrets;

		private:
			PlacementArea::P Place(int x, int y);

		public:
			Level(gfx::Scene::P scene, GameResources& gameResources, int width, int height);
			static Level::P CreateP(gfx::Scene::P scene, GameResources& gameResources, int width, int height);
			static Level::U CreateU(gfx::Scene::P scene, GameResources& gameResources, int width, int height);

			void Update(float delta);

			PlacementArea::P PointedArea(mth::float3 origin, mth::float3 direction);

			void HighlightPath(mth::vec2<int> start, mth::vec2<int> end);
		};
	}
}