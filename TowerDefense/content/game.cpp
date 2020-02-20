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
			m_camera.Update();

			gfx::CB_MatrixBuffer matrixBuffer;
			matrixBuffer.worldMatrix = mth::float4x4::Identity();
			matrixBuffer.cameraMatrix = m_camera.CameraMatrix();
			matrixBuffer.lightMatrix = m_light.LightMatrix();

			gfx::CB_LightBuffer lightBuffer;
			lightBuffer.ambientColor = 0.3f;
			lightBuffer.diffuseColor = 0.7f;
			lightBuffer.sourcePosition = m_light.position;
			lightBuffer.shadowMapDelta = 1.0f / m_shadowMap.Size();
			lightBuffer.eyePosition = m_camera.position;
			lightBuffer.screenSize = mth::float2(static_cast<float>(m_graphics.Width()), static_cast<float>(m_graphics.Height()));

			m_shadowMap.SetAsRenderTarget(m_graphics);
			for (auto& e : m_level->RenderedEntities())
			{
				mth::float4x4 lightMatrix = matrixBuffer.lightMatrix * e->WorldMatrix();
				m_shadowMap.WriteBuffer(m_graphics, &lightMatrix);
				e->Model()->RenderAll(m_graphics);
			}

			m_ambeintOcclusion.SetAsRenderTarget(m_graphics, m_camera);
			for (auto& e : m_level->RenderedEntities())
			{
				m_ambeintOcclusion.WriteBuffer(m_graphics, m_camera, *e);
				e->Model()->RenderAll(m_graphics);
			}
			m_ambeintOcclusion.RenderOcclusionMap(m_graphics, m_camera);

			m_graphics.SetScreenAsRenderTarget();

			m_graphics.WriteVSMatrixBuffer(&matrixBuffer);
			m_graphics.WritePSLightBuffer(&lightBuffer);
			m_shadowMap.SetTextureToRender(m_graphics, 2);
			m_ambeintOcclusion.SetTextureToRender(m_graphics, 3);

			m_graphics.Clear(0.3f, 0.4f, 0.75f, 1.0f);
			auto& allEntities = m_level->RenderedEntities();
			for (auto& e : allEntities)
				e->Render(m_graphics, matrixBuffer);
			if (m_movingTurret)
				m_movingTurret->Render(m_graphics, matrixBuffer);
			m_graphics.Present();
		}
		Game::Game(gfx::Graphics& graphics, gfx::ShadowMap& shadowMap, gfx::AmbientOcclusion& ambeintOcclusion) :
			m_graphics(graphics),
			m_shadowMap(shadowMap),
			m_ambeintOcclusion(ambeintOcclusion),
			m_gameResources(graphics),
			m_level(Level::CreateP(m_gameResources, 11, 9)),
			m_cameraController(m_camera),
			m_timeCounter(0.0f),
			m_secondCounter(0)
		{
			m_cameraController.SetControllerData(mth::float3(-5.5f, 0.0f, -4.5f), 15.0f, mth::float3(0.3f, -0.3f, 0.0f));
			m_cameraController.UpdateTarget();
			m_light.position = mth::float3(12.0f, 13.0f, -3.0f);
			m_light.rotation = mth::float3(1.0f, -1.0f, 0.0f);

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
		Game::P Game::CreateP(gfx::Graphics& graphics, gfx::ShadowMap& shadowMap, gfx::AmbientOcclusion& ambeintOcclusion)
		{
			return std::make_shared<Game>(graphics, shadowMap, ambeintOcclusion);
		}
		Game::U Game::CreateU(gfx::Graphics& graphics, gfx::ShadowMap& shadowMap, gfx::AmbientOcclusion& ambeintOcclusion)
		{
			return std::make_unique<Game>(graphics, shadowMap, ambeintOcclusion);
		}
		void Game::MouseDown(mth::float2 cursor, bool lButtonDown, bool mButtonDown, bool rButtonDown)
		{
			if (lButtonDown && !m_movingTurret)
			{
				mth::float3 origin = m_camera.position;
				mth::float3 dir = m_camera.DirectionFromPoint(m_cursor, m_windowSize);
				Concurrency::critical_section::scoped_lock lock(m_graphics.GetLock());
				m_movingTurret = m_level->PointedTurret(origin, dir);
				if (m_movingTurret)
					m_level->DestroyTurret(m_movingTurret->MapPosition());
			}
		}
		void Game::MouseMove(mth::float2 cursor, bool lButtonDown, bool mButtonDown, bool rButtonDown)
		{
			m_cameraController.MouseMove(cursor, rButtonDown, mButtonDown);
			m_cursor = cursor;
		}
		void Game::MouseUp(mth::float2 cursor, bool lButtonDown, bool mButtonDown, bool rButtonDown)
		{
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