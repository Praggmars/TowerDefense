#include "pch.h"
#include "game.h"
#include <chrono>

namespace TowerDefense
{
	namespace content
	{
		void Game::CreateScene()
		{
			float size = 0.45f;
			m_scene->BackgroundColor(mth::float4(0.8f, 0.4f, 0.2f, 1.0f));
			//Platform::String^ appInstallFolder = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
			//std::wstring filename(appInstallFolder->Begin(), appInstallFolder->End());
			gfx::Texture::P texture = gfx::Texture::CreateColoredTexture(m_graphics, hlp::Color{ 0, 0, 0, 255 }, 16);
			gfx::Texture::P normalmap = gfx::Texture::CreateDefaultNormalmap(m_graphics, 16);
			gfx::ModelLoader ml;
			ml.CreateQuad(mth::float2(-size), mth::float2(size * 2.0f), 0.0f);
			//ml.CreateCube(mth::float3(-size), mth::float3(size * 2.0f));
			gfx::Model::P model = gfx::Model::CreateP(m_graphics, ml);
			phy::Hitbox::P hitbox = phy::Hitbox::CreateP(ml);
			m_scene->Camera().position = mth::float3(2.0f, 5.2f, -10.0f);
			m_scene->Camera().rotation = mth::float3(0.6f, -0.2f, 0.0f);

			float from = -5.0f, to = 5.0f;
			for (float z = from; z <= to; z++)
			{
				for (float x = from; x <= to; x++)
				{
					gfx::Material::P material = gfx::Material::CreateP(texture, normalmap);
					material->MaterialBuffer().color = mth::float4((x + 5.0f) / 10.0f, 1.0f, (z + 5.0f) / 10.0f, 1.0f);
					material->MaterialBuffer().textureWeight = 0.0f;
					gfx::Entity::P e = gfx::Entity::CreateP(m_graphics, model, &material, 1, hitbox);
					e->position.x = x;
					e->position.z = z;
					m_scene->AddEntity(e);
					m_baseEntities.push_back(e);
				}
			}
		}
		void Game::Update(float deltaTime)
		{
			m_cameraController.Update(deltaTime);
			ColorEntitiesByPointing();
		}
		void Game::Render()
		{
			m_scene->Render(m_graphics);
		}
		void Game::ColorEntitiesByPointing()
		{
			mth::float3 origin = m_scene->Camera().position;
			mth::float3 dir = m_scene->Camera().DirectionFromPoint(m_cursor, m_windowSize);
			float minDistance = INFINITY;
			m_selectedEntity = nullptr;

			for (auto& e : m_scene->Entities())
				for (auto& m : e->Materials())
					m->MaterialBuffer().textureWeight = 0.0f;

			if (m_movingEntity)
			{
				for (auto& e : m_baseEntities)
				{
					if (e->Hitbox())
					{
						float distance = e->Hitbox()->DistanceInDirection(origin, dir, e->WorldMatrix());
						if (distance < minDistance)
						{
							minDistance = distance;
							m_selectedEntity = e;
						}
					}
				}

				m_movingEntity->Visible(false);
				if (m_selectedEntity)
				{
					for (auto& m : m_selectedEntity->Materials())
						m->MaterialBuffer().textureWeight = 0.75f;

					m_movingEntity->Visible(true);
					m_movingEntity->position = m_selectedEntity->position;
					m_movingEntity->position.y += 1.0f;
				}
			}
			else
			{
				for (auto& e : m_placedEntities)
				{
					if (e->Hitbox())
					{
						float distance = e->Hitbox()->DistanceInDirection(origin, dir, e->WorldMatrix());
						if (distance < minDistance)
						{
							minDistance = distance;
							m_selectedEntity = e;
						}
					}
				}
				if (m_selectedEntity)
				{
					for (auto& m : m_selectedEntity->Materials())
						m->MaterialBuffer().textureWeight = 0.75f;
				}
			}
		}
		Game::Game(gfx::Graphics& graphics) :
			m_graphics(graphics),
			m_scene(gfx::Scene::CreateP(graphics)),
			m_cameraController(m_scene->Camera())
		{
			CreateScene();

			auto workItemHandler = ref new Windows::System::Threading::WorkItemHandler(
				[this](Windows::Foundation::IAsyncAction^ action)
				{
					auto prevTime = std::chrono::steady_clock::now();
					while (action->Status == Windows::Foundation::AsyncStatus::Started)
					{
						Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());
						auto currentTime = std::chrono::steady_clock::now();
						float deltaTime = std::chrono::duration<float>(currentTime - prevTime).count();
						prevTime = currentTime;

						Update(deltaTime);
						Render();
					}
				});

			m_mainLoopWorker = Windows::System::Threading::ThreadPool::RunAsync(
				workItemHandler,
				Windows::System::Threading::WorkItemPriority::High,
				Windows::System::Threading::WorkItemOptions::TimeSliced);
		}
		Game::P Game::CreateP(gfx::Graphics& graphics)
		{
			return std::make_shared<Game>(graphics);
		}
		Game::U Game::CreateU(gfx::Graphics& graphics)
		{
			return std::make_unique<Game>(graphics);
		}
		void Game::KeyDown(Windows::System::VirtualKey key)
		{
			m_cameraController.KeyDown(key);
		}
		void Game::KeyUp(Windows::System::VirtualKey key)
		{
			m_cameraController.KeyUp(key);

			Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());
			if (key == Windows::System::VirtualKey::P)
			{
				if (m_movingEntity)
				{
					if (m_selectedEntity)
					{
						m_placedEntities.push_back(m_movingEntity);
						m_movingEntity = nullptr;
					}
				}
				else
				{
					if (m_selectedEntity)
					{
						m_scene->RemoveEntity(m_selectedEntity);
						for (auto e = m_placedEntities.begin(); e != m_placedEntities.end(); e++)
						{
							if (*e == m_selectedEntity)
							{
								m_placedEntities.erase(e);
								break;
							}
						}
						m_selectedEntity = nullptr;
					}
				}
			}
		}
		void Game::MouseDown(mth::float2 cursor)
		{
			m_cameraController.MouseDown(cursor);
		}
		void Game::MouseMove(mth::float2 cursor)
		{
			m_cameraController.MouseMove(cursor);
			m_cursor = cursor;
		}
		void Game::MouseUp(mth::float2 cursor)
		{
			m_cameraController.MouseUp();
		}
		void Game::WindowSizeChanged(mth::float2 size)
		{
			m_windowSize = size;
			m_scene->Camera().SetScreenAspect(gfx::Camera::ToScreenAspect(size.x, size.y));
		}
		void Game::PlaceCube()
		{
			Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());

			m_scene->RemoveEntity(m_movingEntity);
			gfx::ModelLoader ml;
			ml.CreateCube(mth::float3(-0.4f), mth::float3(0.8f));

			gfx::Material::P material = gfx::Material::CreateP(
				gfx::Texture::CreateColoredTexture(m_graphics, { 0, 0, 0, 255 }, 16),
				gfx::Texture::CreateDefaultNormalmap(m_graphics, 16));
			m_movingEntity = gfx::Entity::CreateP(m_graphics, gfx::Model::CreateP(m_graphics, ml), &material, 1, phy::Hitbox::CreateP(ml));
			m_scene->AddEntity(m_movingEntity);
		}
	}
}