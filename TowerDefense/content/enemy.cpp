#include "pch.h"
#include "level.h"

namespace TowerDefense
{
	namespace content
	{
		void Enemy::UpdateMaterial()
		{
			float tw = m_health / m_maxHealth;
			tw = 0.25f + 0.75f * tw;
			for (auto& m : Materials())
				m.MaterialBuffer().textureWeight = tw;
		}
		Enemy::Enemy(GameResources& resources, float health) :
			GameObject(resources.enemy),
			m_pathFinder(),
			m_speed(2.0f),
			m_pathProgress(0),
			m_maxHealth(health),
			m_health(health)
		{
			scale = 0.5f;
			rotation.y = mth::pi * -0.5f;

			for (auto& m : Materials())
				m.MaterialBuffer().diffuseColor = mth::float4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		Enemy::P Enemy::CreateP(GameResources& resources, float health)
		{
			return std::make_shared<Enemy>(resources, health);
		}
		Enemy::U Enemy::CreateU(GameResources& resources, float health)
		{
			return std::make_unique<Enemy>(resources, health);
		}
		void Enemy::Update(float deltaTime, Level& level)
		{
			UpdateMaterial();
			mth::float3 startPos = position;
			while (m_pathProgress < m_pathFinder.Path().size() && deltaTime > 0.0f)
			{
				mth::float2 nextPoint = level.MapToWorld(m_pathFinder.Path()[m_pathProgress]->position);
				mth::float3 to(nextPoint.x, position.y, nextPoint.y);
				mth::float3 dir = to - position;
				float distance = dir.Length();
				if (distance > m_speed * deltaTime)
				{
					position += dir * (m_speed * deltaTime / distance);
					break;
				}
				position = to;
				deltaTime -= distance / m_speed;
				m_pathProgress++;
			}
			float weight = std::pow(0.1f, deltaTime);
			rotation.y = rotation.y * weight + std::atan2(startPos.x - position.x, startPos.z - position.z) * (1.0f - weight);
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