#include "pch.h"
#include "level.h"

namespace TowerDefense
{
	namespace content
	{
		Level::Level(gfx::Graphics& graphics, GameResources& gameResources, int width, int height) :
			m_places(width, height),
			m_enemyStartPoint{ 0, height / 2 },
			m_enemyEndPoint{ width - 1, height / 2 }
		{
			CreateLevelMap(graphics, gameResources, width, height);
		}
		Level::P Level::CreateP(gfx::Graphics& graphics, GameResources& gameResources, int width, int height)
		{
			return std::make_shared<Level>(graphics, gameResources, width, height);
		}
		Level::U Level::CreateU(gfx::Graphics& graphics, GameResources& gameResources, int width, int height)
		{
			return std::make_unique<Level>(graphics, gameResources, width, height);
		}
		void Level::Update(float delta)
		{
			for (auto& e : m_enemies)
				e->Update(delta, *this);

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
			return CoordTransform(mth::float2(groundPoint.x, groundPoint.z));
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
		void Level::CreateLevelMap(gfx::Graphics& graphics, GameResources& gameResources, int width, int height)
		{
			Image map((graphics.AppDirectory() + L"resources\\map.png").c_str());
			m_places.resize(map.Width(), map.Height());

			m_levelMap = GameObject::CreateP(gameResources.map);
			m_base = GameObject::CreateP(gameResources.base);

			AddRenderedEntity(*m_levelMap);
			AddRenderedEntity(*m_base);
		}
		alg::Point Level::CoordTransform(mth::float2 p)
		{
			return alg::Point{
				static_cast<int>(p.x + static_cast<float>(m_places.width()) * 0.5f),
				static_cast<int>(p.y + static_cast<float>(m_places.height()) * 0.5f)
			};
		}
		mth::float2 Level::CoordTransform(alg::Point p)
		{
			return mth::float2(
				static_cast<float>(p.x) - static_cast<float>(m_places.width()) * 0.5f + 0.5f,
				static_cast<float>(p.y) - static_cast<float>(m_places.height()) * 0.5f + 0.5f
			);
		}
		void Level::AddRenderedEntity(gfx::Entity& entity)
		{
			m_renderedEntities.push_back(&entity);
		}
		void Level::RemoveRenderedEntity(gfx::Entity& entity)
		{
			gfx::Entity* toRemove = &entity;
			auto iter = m_renderedEntities.begin();
			while (iter != m_renderedEntities.end())
			{
				gfx::Entity* current = *iter;
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
			for (int y = 0; y < m_places.height(); y++)
			{
				for (int x = 0; x < m_places.width(); x++)
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
			auto p = CoordTransform(m_enemyStartPoint);
			enemy->position = mth::float3(p.x, 0.5f, p.y);
			enemy->PathFinder().Resize(static_cast<unsigned>(m_places.width()), static_cast<unsigned>(m_places.height()));
			UpdatePathFinder(enemy->PathFinder());
			enemy->StartPath(m_enemyStartPoint, m_enemyEndPoint);
			m_enemies.push_back(enemy);
			AddRenderedEntity(*enemy);
		}
	}
}