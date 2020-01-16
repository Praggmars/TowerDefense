#include "pch.h"
#include "enemy.h"

namespace TowerDefense
{
	namespace content
	{
		Enemy::Enemy(GameResources& resources) :
			GameObject(resources.enemy),
			m_pathFinder(),
			m_speed(2.0f),
			m_pathProgress(0),
			m_health(100) {}
		Enemy::P Enemy::CreateP(GameResources& resources)
		{
			return std::make_shared<Enemy>(resources);
		}
		Enemy::U Enemy::CreateU(GameResources& resources)
		{
			return std::make_unique<Enemy>(resources);
		}
		void Enemy::Update(float deltaTime)
		{
			while (m_pathFinder.PathLength() > -1 && m_pathProgress < m_pathFinder.PathLength() && deltaTime > 0.0f)
			{
				alg::Point nextPoint = m_pathFinder.Path()[m_pathProgress];
				mth::float3 to(nextPoint.x, position.y, nextPoint.y);
				mth::float3 dir = to - position;
				float distance = dir.Length();
				if (distance > m_speed* deltaTime)
				{
					position += dir * (m_speed * deltaTime / distance);
					return;
				}
				position = to;
				deltaTime -= distance / m_speed;
				m_pathProgress++;
				m_current = nextPoint;
			}
		}
		void Enemy::StartPath(alg::Point start, alg::Point end)
		{
			m_current = start;
			RestartPath(end);
		}
		void Enemy::RestartPath(alg::Point end)
		{
			m_pathProgress = 1;
			m_pathFinder.FindPath(m_current, end);
		}
		bool Enemy::Finished()
		{
			return m_pathProgress >= m_pathFinder.PathLength();
		}
		void Enemy::Damage(unsigned dmg)
		{
			if (m_health < dmg)
				m_health = 0;
			else
				m_health -= dmg;
		}
		bool Enemy::Alive()
		{
			return m_health > 0;
		}
	}
}