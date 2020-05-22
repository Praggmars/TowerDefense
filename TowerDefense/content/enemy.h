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
			float m_maxHealth;
			float m_health;

		private:
			void UpdateMaterial();

		public:
			Enemy(GameResources& resources, float health);
			static Enemy::P CreateP(GameResources& resources, float health);
			static Enemy::U CreateU(GameResources& resources, float health);

			void Update(float deltaTime, Level& level);
			void StartPath();
			bool Finished();

			void Damage(unsigned dmg);
			bool Alive();

			inline alg::PathFinderDiag& PathFinder() { return m_pathFinder; }
		};
	}
}