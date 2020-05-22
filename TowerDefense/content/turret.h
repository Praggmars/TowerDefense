#pragma once

#include "graphics/entity.h"
#include "contentcommon.h"
#include "algorithm/pathfindergrid.h"

namespace TowerDefense
{
	namespace content
	{
		class Enemy;

		struct AttackData
		{
			float damage;
			float reloadTime;
			float range;

			static AttackData Light();
			static AttackData Heavy();
			static AttackData Launcher();
		};

		class Turret :public GameObject
		{
		public:
			SMART_PTR(Turret)
		private:
			alg::Point m_mapPosition;
			AttackData m_attackData;
			float m_timeToShoot;

		private:
			virtual void Face(mth::Positionf& target);

		public:
			Turret(GameResources::GameModel& gameModel, AttackData attackData);
			static Turret::P CreateP(GameResources::GameModel& gameModel, AttackData attackData);
			static Turret::U CreateU(GameResources::GameModel& gameModel, AttackData attackData);

			void Update(float deltaTime);
			void Shoot(std::shared_ptr<Enemy> enemies[], size_t enemyCount);

			inline alg::Point MapPosition() { return m_mapPosition; }
			inline void Place(alg::Point mapPosition) { m_mapPosition = mapPosition; }
		};
	}
}