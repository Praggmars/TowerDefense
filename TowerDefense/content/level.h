#pragma once

#include "turret.h"
#include "enemy.h"
#include "graphics/scene.h"
#include "common/array2d.hpp"
#include "contentcommon.h"
#include <optional>

namespace TowerDefense
{
	namespace content
	{
		class Level
		{
		public:
			SMART_PTR(Level)
		private:
			struct MapElement
			{
				Turret::P turret;
				alg::Point mapPosition;
			};
			GameObject::P m_levelMap;
			Array2D<MapElement> m_places;
			std::vector<Enemy::P> m_enemies;

			alg::Point m_enemyStartPoint;
			alg::Point m_enemyEndPoint;

			std::vector<gfx::Entity*> m_renderedEntities;

		private:
			void AddRenderedEntity(gfx::Entity& entity);
			void RemoveRenderedEntity(gfx::Entity& entity);
			void RegeneratePaths();
			void UpdatePathFinder(alg::PathFinder& pathFinder);

		public:
			Level(GameResources& gameResources, int width, int height);
			static Level::P CreateP(GameResources& gameResources, int width, int height);
			static Level::U CreateU(GameResources& gameResources, int width, int height);

			void Update(float delta);

			void PlaceTurret(alg::Point mapPosition, Turret::P turret);
			void DestroyTurret(alg::Point mapPosition);
			void SpawnEnemy(Enemy::P enemy);

			std::optional<alg::Point> PointedArea(mth::float3 origin, mth::float3 direction);
			Turret::P PointedTurret(mth::float3 origin, mth::float3 direction);
			Enemy::P PointedEnemy(mth::float3 origin, mth::float3 direction);
			GameObject::P PointedTurretOrEnemy(mth::float3 origin, mth::float3 direction);

			inline std::vector<gfx::Entity*>& RenderedEntities() { return m_renderedEntities; }
		};
	}
}