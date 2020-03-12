#pragma once

#include "turret.h"
#include "enemy.h"
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
				enum class Obstacle
				{
					FREE,
					WALL,
					SPAWN,
					GOAL
				};
				alg::Point mapPosition;
				Obstacle obstacle;
			};
			GameObject::P m_levelMap;
			GameObject::P m_base;

			std::vector<alg::Point> m_enemyStartPoints;
			std::vector<alg::Point> m_enemyEndPoints;
			std::vector<alg::Point> m_corners;
			Array2D<MapElement> m_places;
			std::vector<Turret::P> m_turrets;
			std::vector<Enemy::P> m_enemies;

			std::vector<gfx::Entity*> m_renderedEntities;

		private:
			void CreateLevelMap(gfx::Graphics& graphics, GameResources& resources, int width, int height);

			void AddRenderedEntity(gfx::Entity& entity);
			void RemoveRenderedEntity(gfx::Entity& entity);
			void RegeneratePaths();
			void UpdatePathFinder(alg::PathFinder& pathFinder);

		public:
			Level(gfx::Graphics& graphics, GameResources& gameResources, int width, int height);
			static Level::P CreateP(gfx::Graphics& graphics, GameResources& gameResources, int width, int height);
			static Level::U CreateU(gfx::Graphics& graphics, GameResources& gameResources, int width, int height);

			void Update(float delta);

			void PlaceTurret(alg::Point mapPosition, Turret::P turret);
			void DestroyTurret(alg::Point mapPosition);
			void SpawnEnemy(Enemy::P enemy);

			alg::Point TurretCoordTransform(mth::float2 p);
			mth::float2 TurretCoordTransform(alg::Point p);
			alg::Point EnemyCoordTransform(mth::float2 p);
			mth::float2 EnemyCoordTransform(alg::Point p);
			std::optional<alg::Point> PointedArea(mth::float3 origin, mth::float3 direction);
			std::optional<mth::float2> PointedPosition(mth::float3 origin, mth::float3 direction);
			bool CanPlace2x2(alg::Point mapPosition);
			Turret::P PointedTurret(mth::float3 origin, mth::float3 direction);
			Enemy::P PointedEnemy(mth::float3 origin, mth::float3 direction);
			GameObject::P PointedTurretOrEnemy(mth::float3 origin, mth::float3 direction);

			inline std::vector<gfx::Entity*>& RenderedEntities() { return m_renderedEntities; }
			inline alg::Point MapSize() { return { static_cast<int>(m_places.width()), static_cast<int>(m_places.height()) }; }
		};
	}
}