#include "pch.h"
#include "level.h"

namespace TowerDefense
{
	namespace content
	{
		Level::Level(gfx::Graphics& graphics, GameResources& gameResources, int width, int height) :
			m_places(width, height)
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

			for (auto& t : m_turrets)
			{
				t->Update(delta);
				t->Shoot(m_enemies.data(), m_enemies.size());
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
			return TurretCoordTransform(mth::float2(groundPoint.x, groundPoint.z));
		}
		std::optional<mth::float2> Level::PointedPosition(mth::float3 origin, mth::float3 direction)
		{
			float distance = m_levelMap->Hitbox()->DistanceInDirection(origin, direction, m_levelMap->WorldMatrix());
			if (distance == NAN || distance == INFINITY || distance == -INFINITY)
				return {};
			mth::float3 groundPoint = origin + distance * direction;
			return mth::float2(groundPoint.x, groundPoint.z);
		}
		bool Level::CanPlace2x2(alg::Point mapPosition)
		{
			return
				m_places(mapPosition.x + 0, mapPosition.y + 0).obstacle == MapElement::Obstacle::FREE &&
				m_places(mapPosition.x + 0, mapPosition.y + 1).obstacle == MapElement::Obstacle::FREE &&
				m_places(mapPosition.x + 1, mapPosition.y + 0).obstacle == MapElement::Obstacle::FREE &&
				m_places(mapPosition.x + 1, mapPosition.y + 1).obstacle == MapElement::Obstacle::FREE;
		}
		Turret::P Level::PointedTurret(mth::float3 origin, mth::float3 direction)
		{
			Turret::P turret;
			float minDistance = INFINITY;
			for (auto& t : m_turrets)
			{
				if (t->Hitbox())
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
			for (auto& t : m_turrets)
			{
				if (t->Hitbox())
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
			turret->Place(mapPosition);
			m_places(mapPosition.x + 0, mapPosition.y + 0).obstacle = MapElement::Obstacle::WALL;
			m_places(mapPosition.x + 0, mapPosition.y + 1).obstacle = MapElement::Obstacle::WALL;
			m_places(mapPosition.x + 1, mapPosition.y + 0).obstacle = MapElement::Obstacle::WALL;
			m_places(mapPosition.x + 1, mapPosition.y + 1).obstacle = MapElement::Obstacle::WALL;
			m_turrets.push_back(turret);
			AddRenderedEntity(*turret);
			RegeneratePaths();
		}
		void Level::DestroyTurret(alg::Point mapPosition)
		{
			for (auto& t : m_turrets)
			{
				if (t->MapPosition().x == mapPosition.x && t->MapPosition().y == mapPosition.y)
				{
					RemoveRenderedEntity(*t);
					m_places(mapPosition.x + 0, mapPosition.y + 0).obstacle = MapElement::Obstacle::FREE;
					m_places(mapPosition.x + 0, mapPosition.y + 1).obstacle = MapElement::Obstacle::FREE;
					m_places(mapPosition.x + 1, mapPosition.y + 0).obstacle = MapElement::Obstacle::FREE;
					m_places(mapPosition.x + 1, mapPosition.y + 1).obstacle = MapElement::Obstacle::FREE;
					RegeneratePaths();
					break;
				}
			}
		}
		void Level::CreateLevelMap(gfx::Graphics& graphics, GameResources& gameResources, int width, int height)
		{
			Image map((graphics.AppDirectory() + L"resources\\map.png").c_str());
			m_places.resize(map.Width(), map.Height());
			for (int y = 0; y < static_cast<int>(map.Height()); y++)
			{
				for (int x = 0; x < static_cast<int>(map.Width()); x++)
				{
					Color color = map(x, y);
					if (color == Color::Green())
					{
						m_places(x, y).obstacle = MapElement::Obstacle::FREE;
					}
					else if (color == Color::Red())
					{
						m_places(x, y).obstacle = MapElement::Obstacle::SPAWN;
						m_enemyStartPoints.push_back(alg::Point{ x, y });
					}
					else if (color == Color::Blue())
					{
						m_places(x, y).obstacle = MapElement::Obstacle::GOAL;
						m_enemyEndPoints.push_back(alg::Point{ x, y });
					}
					else if (color == Color::Yellow())
					{
						m_places(x, y).obstacle = MapElement::Obstacle::FREE;
						m_corners.push_back(alg::Point{ x, y });
					}
					else
					{
						m_places(x, y).obstacle = MapElement::Obstacle::WALL;
					}
				}
			}
			m_levelMap = GameObject::CreateP(gameResources.map);
			m_base = GameObject::CreateP(gameResources.base);

			AddRenderedEntity(*m_levelMap);
			AddRenderedEntity(*m_base);
		}
		alg::Point Level::TurretCoordTransform(mth::float2 p)
		{
			return alg::Point{
				static_cast<int>(p.x + 24.5f),
				static_cast<int>(-p.y + 18.5f)
			};
		}
		mth::float2 Level::TurretCoordTransform(alg::Point p)
		{
			return mth::float2(
				static_cast<float>(p.x) - 24.0f,
				static_cast<float>(-p.y) + 18.0f
			);
		}
		alg::Point Level::EnemyCoordTransform(mth::float2 p)
		{
			return alg::Point{
				static_cast<int>(p.x + 23.5f),
				static_cast<int>(-p.y + 18.5f)
			};
		}
		mth::float2 Level::EnemyCoordTransform(alg::Point p)
		{
			return mth::float2(
				static_cast<float>(p.x) - 24.5f,
				static_cast<float>(-p.y) + 18.5f
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
				e->RestartPath(m_enemyEndPoints.data(), static_cast<unsigned>(m_enemyEndPoints.size()));
			}
		}
		void Level::UpdatePathFinder(alg::PathFinder& pathFinder)
		{
			for (int y = 0; y < m_places.height(); y++)
			{
				for (int x = 0; x < m_places.width(); x++)
				{
					if (m_places(x, y).obstacle == MapElement::Obstacle::WALL)
						pathFinder.Block(x, y);
					else
						pathFinder.Unblock(x, y);
				}
			}
		}
		void Level::SpawnEnemy(Enemy::P enemy)
		{
			alg::Point enemyStartPoint = m_enemyStartPoints[rand() % m_enemyStartPoints.size()];
			mth::float2 p = EnemyCoordTransform(enemyStartPoint);
			enemy->position = mth::float3(p.x, 0.5f, p.y);
			enemy->PathFinder().Resize(static_cast<unsigned>(m_places.width()), static_cast<unsigned>(m_places.height()));
			UpdatePathFinder(enemy->PathFinder());
			enemy->StartPath(enemyStartPoint, m_enemyEndPoints.data(), static_cast<unsigned>(m_enemyEndPoints.size()));
			m_enemies.push_back(enemy);
			AddRenderedEntity(*enemy);
		}
	}
}