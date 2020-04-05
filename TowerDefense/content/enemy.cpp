#include "pch.h"
#include "level.h"

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
		void Enemy::Update(float deltaTime, Level& level)
		{
			while (m_pathProgress < m_pathFinder.Path().size() && deltaTime > 0.0f)
			{
				mth::float2 nextPoint = level.MapToWorld(m_pathFinder.Path()[m_pathProgress]->position);
				mth::float3 to(nextPoint.x, position.y, nextPoint.y);
				mth::float3 dir = to - position;
				float distance = dir.Length();
				if (distance > m_speed * deltaTime)
				{
					position += dir * (m_speed * deltaTime / distance);
					return;
				}
				position = to;
				deltaTime -= distance / m_speed;
				m_pathProgress++;
			}
		}
		void Enemy::StartPath()
		{
			m_pathProgress = 0;
			m_pathFinder.ReplaceStart(position.x, position.z);
			m_pathFinder.FindPath();
		}
		bool Enemy::Finished()
		{
			if (m_pathFinder.Path().empty()) return false;
			return m_pathProgress >= m_pathFinder.Path().size();
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