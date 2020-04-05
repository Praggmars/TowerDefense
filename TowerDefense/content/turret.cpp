#include "pch.h"
#include "turret.h"
#include "enemy.h"

namespace TowerDefense
{
	namespace content
	{
		void Turret::Face(mth::Positionf& target)
		{
			rotation.y = std::atan2(position.x - target.position.x, position.z - target.position.z);
		}
		Turret::Turret(GameResources::GameModel& gameModel) :
			GameObject(gameModel),
			m_mapPosition{ -1, -1 },
			m_damage(26),
			m_timeToShoot(0.0f),
			m_reloadTime(0.8f),
			m_range(2.5f) {}
		Turret::P Turret::CreateP(GameResources::GameModel& gameModel)
		{
			return std::make_shared<Turret>(gameModel);
		}
		Turret::U Turret::CreateU(GameResources::GameModel& gameModel)
		{
			return std::make_unique<Turret>(gameModel);
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
				if (e.Alive() && (position - e.position).LengthSquare() < m_range * m_range)
				{
					Face(e);
					if (m_timeToShoot <= 0.0f)
					{
						e.Damage(m_damage);
						m_timeToShoot += m_reloadTime;
					}
					return;
				}
			}

			m_timeToShoot = 0.0f;
		}
	}
}