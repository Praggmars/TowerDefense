#include "pch.h"
#include "game.h"
#include <chrono>

namespace TowerDefense
{
	namespace content
	{
		void Game::Update(float deltaTime)
		{
			m_timeCounter += deltaTime;
			while (m_timeCounter >= 1.0f)
			{
				m_timeCounter -= 1.0f;
				m_secondCounter++;
				m_level->SpawnEnemy(Enemy::CreateP(m_gameResources));
			}

#if FREELOOK_CAMERA
			m_cameraController.Update(deltaTime);
#endif

			m_level->Update(deltaTime);

			mth::float3 origin = m_camera.position;
			mth::float3 dir = m_camera.DirectionFromPoint(m_cursor, m_windowSize);
			if (m_movingTurret)
			{
				m_movingTurret->Visible(false);
				if (auto mapPos = m_level->PointedArea(origin, dir))
				{
					m_movingTurret->Visible(true);
					m_movingTurret->position = mth::float3(mapPos->x, 0.0f, mapPos->y);
				}
			}
			else
			{
				if (auto turret = m_level->PointedTurret(origin, dir))
				{
					for (auto& m : turret->Materials())
						m->MaterialBuffer().textureWeight = 0.25f;
				}
			}
		}
		void Game::Render()
		{
			m_scene->StartRendering(m_camera);
			m_level->Render(*m_scene);
			if (m_movingTurret)
				m_scene->RenderEntity(*m_movingTurret);
			m_scene->EndRendering();
		}
		Game::Game(gfx::Graphics& graphics) :
			m_graphics(graphics),
			m_gameResources(graphics),
			m_scene(gfx::Scene::CreateP(graphics)),
			m_level(Level::CreateP(m_scene, m_gameResources, 11, 9)),
			m_cameraController(m_camera),
			m_timeCounter(0.0f),
			m_secondCounter(0)
		{
#if !FREELOOK_CAMERA
			m_cameraController.SetControllerData(mth::float3(-5.5f, 0.0f, -4.5f), 15.0f, mth::float3(0.3f, -0.3f, 0.0f));
			m_cameraController.UpdateTarget();
#endif
			m_scene->BackgroundColor(mth::float4(0.8f, 0.4f, 0.2f, 1.0f));
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
			if (!m_movingTurret)
			{
				mth::float3 origin = m_camera.position;
				mth::float3 dir = m_camera.DirectionFromPoint(m_cursor, m_windowSize);
				Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());
				m_movingTurret = m_level->PointedTurret(origin, dir);
				m_level->DestroyTurret(m_movingTurret->MapPosition());
			}
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
				mth::float3 origin = m_camera.position;
				mth::float3 dir = m_camera.DirectionFromPoint(m_cursor, m_windowSize);
				Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());
				if (auto mapPos = m_level->PointedArea(origin, dir))
				{
					m_level->PlaceTurret(*mapPos, m_movingTurret);
					m_movingTurret = nullptr;
				}
			}
		}
		void Game::WindowSizeChanged(mth::float2 size)
		{
			m_windowSize = size;
			m_camera.SetScreenAspect(gfx::Camera::ToScreenAspect(size.x, size.y));
		}
		void Game::PlaceTurret()
		{
			Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());

			if (!m_movingTurret)
				m_movingTurret = Turret::CreateP(m_gameResources);
		}
	}
}