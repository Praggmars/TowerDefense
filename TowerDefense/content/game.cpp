#include "pch.h"
#include "game.h"
#include <chrono>

namespace TowerDefense
{
	namespace content
	{
		void Game::Update(float deltaTime)
		{
#if FREELOOK_CAMERA
			m_cameraController.Update(deltaTime);
#endif

			m_level->Update(deltaTime);

			if (m_movingTurret)
			{
				mth::float3 origin = m_scene->Camera().position;
				mth::float3 dir = m_scene->Camera().DirectionFromPoint(m_cursor, m_windowSize);
				auto area = m_level->PointedArea(origin, dir);
				m_movingTurret->Visible(false);
				if (area && !area->HasTurret())
				{
					m_movingTurret->Visible(true);
					for (auto& m : area->Materials())
						m->MaterialBuffer().textureWeight = 0.25f;

					m_movingTurret->position = area->position;
					m_movingTurret->position.y += 0.5f;
				}
			}
			m_level->HighlightPath(mth::vec2<int>(1, 1), mth::vec2<int>(4, 6));
		}
		void Game::Render()
		{
			m_scene->Render(m_graphics);
		}
		Game::Game(gfx::Graphics& graphics) :
			m_graphics(graphics),
			m_gameResources(graphics),
			m_scene(gfx::Scene::CreateP(graphics)),
			m_level(Level::CreateP(m_scene, m_gameResources, 11, 9)),
			m_cameraController(m_scene->Camera())
		{
#if !FREELOOK_CAMERA
			m_cameraController.SetControllerData(mth::float3(), 15.0f, mth::float3(0.3f, -0.3f, 0.0f));
			m_cameraController.UpdateTarget();
#endif

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
		}
		void Game::MouseDown(mth::float2 cursor)
		{
#if FREELOOK_CAMERA
			m_cameraController.MouseDown(cursor);
#endif
		}
		void Game::MouseMove(mth::float2 cursor)
		{
			m_cameraController.MouseMove(cursor);
			m_cursor = cursor;
		}
		void Game::MouseUp(mth::float2 cursor)
		{
#if FREELOOK_CAMERA
			m_cameraController.MouseUp();
#endif
			if (m_movingTurret)
			{
				Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());
				mth::float3 origin = m_scene->Camera().position;
				mth::float3 dir = m_scene->Camera().DirectionFromPoint(m_cursor, m_windowSize);
				auto area = m_level->PointedArea(origin, dir);
				if (area && !area->HasTurret())
				{
					m_movingTurret->position = area->position;
					m_movingTurret->position.y += 0.5f;
					area->PlaceTurret(m_movingTurret);
					m_scene->AddEntity(m_movingTurret);
					m_movingTurret = nullptr;
				}
		}
		}
		void Game::WindowSizeChanged(mth::float2 size)
		{
			m_windowSize = size;
			m_scene->Camera().SetScreenAspect(gfx::Camera::ToScreenAspect(size.x, size.y));
		}
		void Game::PlaceTurret()
		{
			Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());

			if (m_movingTurret)
				m_scene->RemoveEntity(m_movingTurret);

			gfx::Material::P material = gfx::Material::CreateP(
				m_gameResources.turretTexture,
				m_gameResources.turretNormalmap);
			m_movingTurret = Turret::CreateP(m_gameResources.turretModel, &material, 1, m_gameResources.turretHitbox);
			m_scene->AddEntity(m_movingTurret);
		}
	}
}