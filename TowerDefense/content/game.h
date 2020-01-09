#pragma once

#include "level.h"
#include "graphics/firstpersoncontroller.h"
#include "graphics/editorstylecontroller.h"

#define FREELOOK_CAMERA 0

namespace TowerDefense
{
	namespace content
	{
		class Game
		{
		public:
			SMART_PTR(Game)

		private:
			gfx::Graphics& m_graphics;
			GameResources m_gameResources;
			gfx::Scene::P m_scene;
			Level::P m_level;
			Turret::P m_movingTurret;
#if FREELOOK_CAMERA
			gfx::FirstPersonController m_cameraController;
#else
			gfx::EditorStyleController m_cameraController;
#endif
			mth::float2 m_cursor;
			mth::float2 m_windowSize;
			Windows::Foundation::IAsyncAction^ m_mainLoopWorker;

		private:
			void Update(float deltaTime);
			void Render();

		public:
			Game(gfx::Graphics& graphics);
			static Game::P CreateP(gfx::Graphics& graphics);
			static Game::U CreateU(gfx::Graphics& graphics);

			void KeyDown(Windows::System::VirtualKey key);
			void KeyUp(Windows::System::VirtualKey key);
			void MouseDown(mth::float2 cursor);
			void MouseMove(mth::float2 cursor);
			void MouseUp(mth::float2 cursor);
			void WindowSizeChanged(mth::float2 size);

			void PlaceTurret();
		};
	}
}