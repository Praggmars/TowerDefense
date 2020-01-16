#pragma once

#include "graphics/entity.h"
#include "contentcommon.h"
#include "algorithm/pathfinder.h"

namespace TowerDefense
{
	namespace content
	{
		class Enemy;

		class Turret :public GameObject
		{
		public:
			SMART_PTR(Turret)
		private:
			alg::Point m_mapPosition;
			unsigned m_damage;
			float m_timeToShoot;
			float m_reloadTime;
			float m_range;

		public:
			Turret(GameResources& resources);
			static Turret::P CreateP(GameResources& resources);
			static Turret::U CreateU(GameResources& resources);

			void Update(float deltaTime);
			void Shoot(std::shared_ptr<Enemy> enemies[], size_t enemyCount);

			inline alg::Point MapPosition() { return m_mapPosition; }
			inline void Place(alg::Point mapPosition) { m_mapPosition = mapPosition; }
		};
	}
}