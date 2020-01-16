#include "pch.h"
#include "turret.h"
#include "enemy.h"

namespace TowerDefense
{
	namespace content
	{
		Turret::Turret(GameResources& resources) :
			GameObject(resources.turret),
			m_mapPosition{ -1, -1 },
			m_damage(34),
			m_timeToShoot(0.0f),
			m_reloadTime(0.8f),
			m_range(2.5f) {}
		Turret::P Turret::CreateP(GameResources& resources)
		{
			return std::make_shared<Turret>(resources);
		}
		Turret::U Turret::CreateU(GameResources& resources)
		{
			return std::make_unique<Turret>(resources);
		}
		void Turret::Update(float deltaTime)
		{
			if (m_timeToShoot >= 0.0f)
				m_timeToShoot -= deltaTime;
		}
		void Turret::Shoot(std::shared_ptr<Enemy> enemies[], size_t enemyCount)
		{
			if (m_timeToShoot > 0.0f)
				return;

			for (size_t i = 0; i < enemyCount; i++)
			{
				Enemy& e = *enemies[i];
				if (e.Alive() && (position - e.position).LengthSquare() < m_range * m_range)
				{
					e.Damage(m_damage);
					m_timeToShoot += m_reloadTime;
					return;
				}
			}

			m_timeToShoot = 0.0f;
		}
	}
}