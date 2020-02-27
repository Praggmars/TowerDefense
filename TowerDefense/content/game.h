#pragma once

#include "level.h"
#include "graphics/editorstylecontroller.h"
#include "graphics/pointlight.h"
#include "graphics/ambientocclusion.h"
#include "graphics/shadowmap.h"

namespace TowerDefense
{
	namespace content
	{
		enum class TurretType
		{
			LIGHT,
			HEAVY,
			LAUNCHER
		};

		class Game
		{
		public:
			SMART_PTR(Game)

		private:
			gfx::Graphics& m_graphics;
			gfx::ShadowMap& m_shadowMap;
			gfx::AmbientOcclusion& m_ambeintOcclusion;

			GameResources m_gameResources;
			gfx::PointLight m_light;
			gfx::Camera m_camera;
			Level::P m_level;
			Turret::P m_movingTurret;
			gfx::EditorStyleController m_cameraController;
			mth::float2 m_cursor;
			mth::float2 m_windowSize;
			Windows::Foundation::IAsyncAction^ m_mainLoopWorker;

			float m_timeCounter;
			unsigned m_secondCounter;

		private:
			void Update(float deltaTime);
			void Render();

		public:
			Game(gfx::Graphics& graphics, gfx::ShadowMap& shadowMap, gfx::AmbientOcclusion& ambeintOcclusion);
			static Game::P CreateP(gfx::Graphics& graphics, gfx::ShadowMap& shadowMap, gfx::AmbientOcclusion& ambeintOcclusion);
			static Game::U CreateU(gfx::Graphics& graphics, gfx::ShadowMap& shadowMap, gfx::AmbientOcclusion& ambeintOcclusion);

			void MouseDown(mth::float2 cursor, bool lButtonDown, bool mButtonDown, bool rButtonDown);
			void MouseMove(mth::float2 cursor, bool lButtonDown, bool mButtonDown, bool rButtonDown);
			void MouseUp(mth::float2 cursor, bool lButtonDown, bool mButtonDown, bool rButtonDown);
			void MouseWheel(int delta);
			void WindowSizeChanged(mth::float2 size);

			void PlaceTurret(TurretType turretType);
		};
	}
}