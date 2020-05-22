#include "pch.h"
#include "turret.h"
#include "enemy.h"

namespace TowerDefense
{
	namespace content
	{
		AttackData AttackData::Light()
		{
			AttackData ad;
			ad.damage = 13.0f;
			ad.reloadTime = 0.2f;
			ad.range = 2.5f;
			return ad;
		}
		AttackData AttackData::Heavy()
		{
			AttackData ad;
			ad.damage = 210.0f;
			ad.reloadTime = 1.8f;
			ad.range = 4.5f;
			return ad;
		}
		AttackData AttackData::Launcher()
		{
			AttackData ad;
			ad.damage = 96.0f;
			ad.reloadTime = 1.2f;
			ad.range = 7.0f;
			return ad;
		}

		void Turret::Face(mth::Positionf& target)
		{
			rotation.y = std::atan2(position.x - target.position.x, position.z - target.position.z);
		}
		Turret::Turret(GameResources::GameModel& gameModel, AttackData attackData) :
			GameObject(gameModel),
			m_mapPosition{ -1, -1 },
			m_attackData(attackData),
			m_timeToShoot(0.0f) {}
		Turret::P Turret::CreateP(GameResources::GameModel& gameModel, AttackData attackData)
		{
			return std::make_shared<Turret>(gameModel, attackData);
		}
		Turret::U Turret::CreateU(GameResources::GameModel& gameModel, AttackData attackData)
		{
			return std::make_unique<Turret>(gameModel, attackData);
		}
		void Turret::Update(float deltaTime)
		{
			if (m_timeToShoot >= 0.0f)
				m_timeToShoot -= deltaTime;
		}
		void Turret::Shoot(std::shared_ptr<Enemy> enemies[], size_t enemyCount)
		{
			for (size_t i = 0; i < enemyCount; i++)
			{
				Enemy& e = *enemies[i];
				if (e.Alive() && (position - e.position).LengthSquare() < m_attackData.range * m_attackData.range)
				{
					Face(e);
					if (m_timeToShoot <= 0.0f)
					{
						e.Damage(m_attackData.damage);
						m_timeToShoot += m_attackData.reloadTime;
					}
					return;
				}
			}

			m_timeToShoot = 0.0f;
		}
	}
}