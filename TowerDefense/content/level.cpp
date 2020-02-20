#include "pch.h"
#include "level.h"

namespace TowerDefense
{
	namespace content
	{
		Level::Level(GameResources& gameResources, int width, int height) :
			m_levelMap(GameObject::CreateP(gameResources.area)),
			m_places(width, height),
			m_enemyStartPoint{ 0, height / 2 },
			m_enemyEndPoint{ width - 1, height / 2 } 
		{
			AddRenderedEntity(*m_levelMap);
		}
		Level::P Level::CreateP(GameResources& gameResources, int width, int height)
		{
			return std::make_shared<Level>(gameResources, width, height);
		}
		Level::U Level::CreateU(GameResources& gameResources, int width, int height)
		{
			return std::make_unique<Level>(gameResources, width, height);
		}
		void Level::Update(float delta)
		{
			for (auto& e : m_enemies)
				e->Update(delta);

			for (auto& a : m_places)
			{
				if (a.turret)
				{
					for (auto& m : a.turret->Materials())
						m->MaterialBuffer().textureWeight = 1.0f;
					a.turret->Update(delta);
					a.turret->Shoot(m_enemies.data(), m_enemies.size());
				}
			}

			auto iter = m_enemies.begin();
			while (iter != m_enemies.end())
			{
				Enemy::P& e = *iter;
				if (e->Finished() || !e->Alive())
				{
					RemoveRenderedEntity(*e);
					iter = m_enemies.erase(iter);
				}
				else
					iter++;
			}
		}
		std::optional<alg::Point> Level::PointedArea(mth::float3 origin, mth::float3 direction)
		{
			float distance = m_levelMap->Hitbox()->DistanceInDirection(origin, direction, m_levelMap->WorldMatrix());
			if (distance == NAN || distance == INFINITY || distance == -INFINITY)
				return {};
			mth::float3 groundPoint = origin + distance * direction;
			alg::Point p = {
				static_cast<int>(floor(groundPoint.x)),
				static_cast<int>(floor(groundPoint.z))
			};
			return p;
		}
		Turret::P Level::PointedTurret(mth::float3 origin, mth::float3 direction)
		{
			Turret::P turret;
			float minDistance = INFINITY;
			for (auto& a : m_places)
			{
				Turret::P t = a.turret;
				if (t && t->Hitbox())
				{
					float distance = t->Hitbox()->DistanceInDirection(origin, direction, t->WorldMatrix());
					if (distance < minDistance)
					{
						minDistance = distance;
						turret = t;
					}
				}
			}
			return turret;
		}
		Enemy::P Level::PointedEnemy(mth::float3 origin, mth::float3 direction)
		{
			Enemy::P enemy;
			float minDistance = INFINITY;
			for (auto& e : m_enemies)
			{
				if (e->Hitbox())
				{
					float distance = e->Hitbox()->DistanceInDirection(origin, direction, e->WorldMatrix());
					if (distance < minDistance)
					{
						minDistance = distance;
						enemy = e;
					}
				}
			}
			return enemy;
		}
		GameObject::P Level::PointedTurretOrEnemy(mth::float3 origin, mth::float3 direction)
		{
			GameObject::P ret;
			float minDistance = INFINITY;
			for (auto& a : m_places)
			{
				Turret::P t = a.turret;
				if (t && t->Hitbox())
				{
					float distance = t->Hitbox()->DistanceInDirection(origin, direction, t->WorldMatrix());
					if (distance < minDistance)
					{
						minDistance = distance;
						ret = t;
					}
				}
			}
			for (auto& e : m_enemies)
			{
				if (e->Hitbox())
				{
					float distance = e->Hitbox()->DistanceInDirection(origin, direction, e->WorldMatrix());
					if (distance < minDistance)
					{
						minDistance = distance;
						ret = e;
					}
				}
			}
			return ret;
		}
		void Level::PlaceTurret(alg::Point mapPosition, Turret::P turret)
		{
			auto& place = m_places(mapPosition.x, mapPosition.y);
			if (!place.turret)
			{
				place.turret = turret;
				turret->Place(mapPosition);
				AddRenderedEntity(*turret);
				RegeneratePaths();
			}
		}
		void Level::DestroyTurret(alg::Point mapPosition)
		{
			auto& place = m_places(mapPosition.x, mapPosition.y);
			if (place.turret)
			{
				RemoveRenderedEntity(*place.turret);
				place.turret = nullptr;
				RegeneratePaths();
			}
		}
		void Level::AddRenderedEntity(gfx::Entity& entity)
		{
			m_renderedEntities.push_back(&entity);
		}
		void Level::RemoveRenderedEntity(gfx::Entity& entity)
		{
			void* toRemove = &entity;
			auto iter = m_renderedEntities.begin();
			while (iter != m_renderedEntities.end())
			{
				void* current = *iter;
				if (toRemove == current)
				{
					m_renderedEntities.erase(iter);
					return;
				}
				iter++;
			}
		}
		void Level::RegeneratePaths()
		{
			for (auto& e : m_enemies)
			{
				UpdatePathFinder(e->PathFinder());
				e->RestartPath(m_enemyEndPoint);
			}
		}
		void Level::UpdatePathFinder(alg::PathFinder& pathFinder)
		{
			for (size_t y = 0; y < m_places.height(); y++)
			{
				for (size_t x = 0; x < m_places.width(); x++)
				{
					if (m_places(x, y).turret)
						pathFinder.Block(x, y);
					else
						pathFinder.Unblock(x, y);
				}
			}
		}
		void Level::SpawnEnemy(Enemy::P enemy)
		{
			enemy->position = mth::float3(m_enemyStartPoint.x, 0.0f, m_enemyStartPoint.y);
			enemy->PathFinder().Resize(m_places.width(), m_places.height());
			UpdatePathFinder(enemy->PathFinder());
			enemy->StartPath(m_enemyStartPoint, m_enemyEndPoint);
			m_enemies.push_back(enemy);
			AddRenderedEntity(*enemy);
		}
	}
}