#pragma once

#include "graphics/entity.h"
#include "contentcommon.h"
#include "algorithm/pathfinderdiag.h"

namespace TowerDefense
{
	namespace content
	{
		class Level;

		class Enemy :public GameObject
		{
		public:
			SMART_PTR(Enemy)
		private:
			alg::PathFinderDiag m_pathFinder;
			float m_speed;
			unsigned m_pathProgress;
			unsigned m_health;

		public:
			Enemy(GameResources& resources);
			static Enemy::P CreateP(GameResources& resources);
			static Enemy::U CreateU(GameResources& resources);

			void Update(float deltaTime, Level& level);
			void StartPath();
			bool Finished();

			void Damage(unsigned dmg);
			bool Alive();

			inline alg::PathFinderDiag& PathFinder() { return m_pathFinder; }
		};
	}
}