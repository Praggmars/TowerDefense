#pragma once

#include "graphics/entity.h"
#include "contentcommon.h"
#include "algorithm/pathfinder.h"

namespace TowerDefense
{
	namespace content
	{
		class Enemy :public GameObject
		{
		public:
			SMART_PTR(Enemy)
		private:
			alg::PathFinder m_pathFinder;
			float m_speed;
			alg::Point m_current;
			unsigned m_pathProgress;
			unsigned m_health;

		public:
			Enemy(GameResources& resources);
			static Enemy::P CreateP(GameResources& resources);
			static Enemy::U CreateU(GameResources& resources);

			void Update(float deltaTime);
			void StartPath(alg::Point start, alg::Point end);
			void RestartPath(alg::Point end);
			bool Finished();

			void Damage(unsigned dmg);
			bool Alive();

			inline alg::PathFinder& PathFinder() { return m_pathFinder; }
		};
	}
}